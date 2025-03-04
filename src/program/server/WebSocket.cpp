#include "server/WebSocket.hpp"
#include "server/JsonDispatch.hpp"
#include "tas/Record.hpp"
#include "TextWriter.hpp"

#include <lib/base64.hpp>
#include <lib/sha1.hpp>
#include <lib/socket_hack.hpp>
#include <nn/nifm.h>
#include <nn/os.h>
#include <nn/socket.h>
#include <nn/util.h>

namespace lotuskit::server {
    namespace WebSocketImpl::SendFrame {
        inline u8 headerLenFromPayloadLen(u32 pLen) {
            // no masking key, no extension data (for server)
            // https://datatracker.ietf.org/doc/html/rfc6455#section-5.2
            // frame type byte, + 0x80 = message FIN
            // 01 = text frame (2=bin, 8=close), so 0x81 is a 1-frame text message
            // size byte (mask=0 for server). Oversizing is rejected by firefox, so we can't just ignore the small type and only send medium
            // 00 - 7d = 1 - 125B      -- 7 bit payload len (small)
            // 7e      = 126B - 64KB   -- next 2B are payload len (medium)
            // 7f      = >64KB         -- next 8B are payload len (large)
            return pLen <= 125 ? 2 :
                   pLen < 65535 ? 4 : 10;
        }

        inline void serializeHeader(u8* dst, u8 frameType, u8 hLen, u32 pLen) {
            if (hLen == 2) {
                dst[0] = frameType;
                dst[1] = pLen;

            } else if (hLen == 4) {
                dst[0] = frameType;
                dst[1] = 0x7e;
                dst[2] = pLen / 0x100;
                dst[3] = pLen % 0x100;

            } else { // hLen == 10
                dst[0] = frameType;
                dst[1] = 0x7f;
                dst[2] = 0; //pLen / (1 << 8*7); // 64b size
                dst[3] = 0; //pLen % (1 << 8*7) / (1 << 8*6);
                dst[4] = 0; //pLen % (1 << 8*6) / (1 << 8*5);
                dst[5] = 0; //pLen % (1 << 8*5) / (1 << 8*4);
                dst[6] = pLen / (1 << 8*3);
                dst[7] = pLen % (1 << 8*3) / (1 << 8*2);
                dst[8] = pLen % (1 << 8*2) / (1 << 8*1);
                dst[9] = pLen % (1 << 8*1);
            }
        }

        inline u32 getSerializedFrameSize(u8* frame) {
            if (frame[1] < 0x7e) { return 2 + frame[1]; }
            if (frame[1] == 0x7e) { return 4 + frame[2]*0x100 + frame[3]; }
            if (frame[1] == 0x7f) { return 10 + frame[6]*0x1000000 + frame[7]*0x10000 + frame[8]*0x100 + frame[9]; }
            const char* warn = "[ws::getSerializedFrameSize] oh no its sending 0 ";
            svcOutputDebugString(warn, strlen(warn)); // also doesn't seem to happen
            return 0;
        }

        inline const char* enforceAndWarnTextLimits(const char* payloadSus) {
            // this doesn't seem to be a case we run into yet, but i forget what happens
            static const char spaceStr[2] = " ";
            if (payloadSus == nullptr) {
                const char* warn = "[ws::enforceAndWarnTextLimits] promoting nullptr to \" \"";
                svcOutputDebugString(warn, strlen(warn));
                return spaceStr;
            }
            if (*payloadSus == '\0') {
                const char* warn = "[ws::enforceAndWarnTextLimits] promoting \"\" to \" \"";
                svcOutputDebugString(warn, strlen(warn));
                return spaceStr;
            }
            return payloadSus;
        }
    } // ns

    namespace WebSocketImpl::SendQueue {
        nn::os::MutexType enqueueMutex;
        constexpr size_t SEND_QUEUE_SIZE = 0x100000;
        alignas(PAGE_SIZE) u8 sendQueueBuf[SEND_QUEUE_SIZE];
        const u8* sendQueueBufEnd = sendQueueBuf + SEND_QUEUE_SIZE - 1;
        u8* sendQueueHead = sendQueueBuf;
        u8* sendQueueTail = sendQueueBuf;

        void init() {
            std::memset(sendQueueBuf, 0, SEND_QUEUE_SIZE);
            nn::os::InitializeMutex(&enqueueMutex, true, 0);
        }

        inline u8* getNextFrameForEnqueue(u32 fLen) {
            if (sendQueueHead + fLen <= sendQueueBufEnd - 0x10) {
                return sendQueueHead; // not near the end, it fits
            }
            // it won't fit at the end, wrap to beginning
            auto sendQueueBufRemainder = sendQueueBufEnd - sendQueueHead;
            if (sendQueueBufRemainder > 0) {
                // zero everything we're not using -- "terminating" null frameType will indicate no more frames for the send impl
                std::memset(sendQueueHead, 0, sendQueueBufRemainder);
            }
            return sendQueueBuf;
        }

        inline void enqueueText(const char* payloadSus) {
            const char* payload = WebSocketImpl::SendFrame::enforceAndWarnTextLimits(payloadSus);

            nn::os::LockMutex(&enqueueMutex);

            u32 pLen = strlen(payload);
            u8 hLen = SendFrame::headerLenFromPayloadLen(pLen);
            u32 fLen = hLen + pLen; // frame is header + payload

            u8* dst = getNextFrameForEnqueue(fLen);
            SendFrame::serializeHeader(dst, 0x81, hLen, pLen);
            std::memcpy(&dst[hLen], payload, pLen);

            sendQueueHead = (u8*)ALIGN_UP(dst + fLen, 8); // advance ptr to next
            if (sendQueueHead >= sendQueueBufEnd) {
                sendQueueHead = sendQueueBuf; // wrap to beginning
            }

            nn::os::UnlockMutex(&enqueueMutex);
        }

        inline void dequeueFrame(u32 fLen) {
            if (*sendQueueTail == 0x00) {
                sendQueueTail = sendQueueBuf; // wrap to beginning
                return;
            }
            sendQueueTail = (u8*)ALIGN_UP(sendQueueTail + fLen, 8);
            if (sendQueueTail >= sendQueueBufEnd) {
                sendQueueTail = sendQueueBuf; // wrap to beginning
            }
        }
    } // ns

    //namespace WebSocketImpl::PrivateStatic {
        constexpr size_t SOCKET_POOL_SIZE = 0x100000;
        constexpr size_t ALLOCATOR_POOL_SIZE = 0x20000;
        //u8 socketPool[SOCKET_POOL_SIZE + ALLOCATOR_POOL_SIZE] __attribute__((aligned(0x4000))) = {0};
        u8* socketPool = nullptr;
        s32 serverSocketFd;
        s32 clientSocketFd;
        sockaddr serverAddress;
        sockaddr clientAddress;
        u32 clientAddressLen;
        bool isWsEstablished = false;

        constexpr u64 SERVER_LISTEN_COMBO = (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11); // L R ZL ZR + -
        u32 listenReqDebounce = 0;
        u32 listenNotifDelay = 0;

#if WEBSOCKET_DO_THREADED_SEND
        constexpr size_t STACK_SIZE = 0x2000; // XXX idk
        alignas(PAGE_SIZE) u8 sendThreadStack[STACK_SIZE];
#endif
    //}

#if WEBSOCKET_DO_THREADED_SEND
    // sends queued/deferred ws messages. See SendTextBlocking to bypass this and send immediately, eg to get a log out before a known crash
    void WebSocket::threadFuncSend(void* _) {
        char buf[200];
        while (true) {
            if (!isWsEstablished) { continue; } // TODO detect dead ws on send, cleanup
            nn::util::SNPrintf(buf, sizeof(buf), R"({ "threadedTick": "0x%p", "doSend": false })", svcGetSystemTick());
            svcOutputDebugString(buf, strlen(buf));
            //SendMessageFrameImpl(buf); // FIXME can't even send in the thread without crashing? thats not right
            svcSleepThread(1000000000); // 1B ns = 1s
            //nn::os::SleepThread(nn::os::ConvertToTimeSpan(1000000000));
        }
    }
#endif

    // alternative to threadFuncSend, flushing deferred messages
    void WebSocket::flushSendQueueBlocking() {
        if (!isWsEstablished) { return; }
        while (WebSocketImpl::SendQueue::sendQueueTail != WebSocketImpl::SendQueue::sendQueueHead) {
            u32 fLen = WebSocketImpl::SendFrame::getSerializedFrameSize(WebSocketImpl::SendQueue::sendQueueTail);

            // a null frame may terminate the queue if the last element left some remainder. Don't send it, just let dequeueFrame consume it.
            // FIXME better queue impl? its leaking. this still might drop 1 char messages? oh well
            if (fLen > 3) {
                //lotuskit::TextWriter::printf(0, "\n[ws::send] fLen %d at %p (sendq %p - %p): \n", fLen, WebSocketImpl::SendQueue::sendQueueTail, WebSocketImpl::SendQueue::sendQueueBuf, WebSocketImpl::SendQueue::sendQueueBufEnd);
                //lotuskit::HexDump::textwriter_printf_u8(0, WebSocketImpl::SendQueue::sendQueueTail, WebSocketImpl::SendQueue::sendQueueTail, 20); // 0x200 after
                nn::socket::Send(clientSocketFd, WebSocketImpl::SendQueue::sendQueueTail, fLen, 0); // blocking
            }

            WebSocketImpl::SendQueue::dequeueFrame(fLen);
        }
    }

    void WebSocket::sendTextNoblock(const char* payloadSus) {
        if (!isWsEstablished) { return; }
        WebSocketImpl::SendQueue::enqueueText(payloadSus); // (it can still block on the mutex, TODO look into lock free wizard shit)
    }

    void WebSocket::sendTextBlocking(const char* payloadSus) {
        if (!isWsEstablished) { return; }
        const char* payload = WebSocketImpl::SendFrame::enforceAndWarnTextLimits(payloadSus);

        // TODO fd mutex? I'm unclear how all this behaves + where this giant socketPool fits into things...
        // (Is it copying my data into there? Does it do any locking of its own? If it's blocking, shouldn't it use the pointer we call send with, instead of copying?)
        // but ig it still needs to tcp encapsulate, too bad

        // plaintext json only. its tempting to use some fancy binary format but:
        // - i like being able to read the wire in devtools network tab
        // - interop
        // - size difference isnt meaningful at reasonable msg volumes
        // - i dont care about decode speed in the mod, busy workloads should be builtin or script driven
        // - for hot logging paths, manually encoding simple json will be faster than a lib anyways, and textwriter or files may be more appropriate
        // - in the browser, plain json is usually just as fast as binary formats. this one is asmjs tho https://github.com/artcompiler/L16/blob/master/src/ubjson.js
        // so lets not deal with it unless we really need it
        u32 pLen = strlen(payload);
        u8 hLen = WebSocketImpl::SendFrame::headerLenFromPayloadLen(pLen);
        u32 fLen = hLen + pLen; // frame is header + payload

        // XXX this func is not well exercised
        const char* warn = "[ws::sendTextBlocking] uhh";
        svcOutputDebugString(warn, strlen(warn));
        if (fLen < 4) {
            return; // XXX also untested, but refusing to send empty frames should be fine?
        }

        char buf[fLen];
        WebSocketImpl::SendFrame::serializeHeader((u8*)buf, 0x81, hLen, pLen);
        std::memcpy(&buf[hLen], payload, pLen);
        nn::socket::Send(clientSocketFd, buf, fLen, 0); // blocking
    }

    void WebSocket::recvNoblockAndProc() {
        if (!isWsEstablished) { return; }
        u8 frameType__sizeByte__12more[14]; // minimum supported frame length (client ws frames can go as short as 6B)
        constexpr auto NOBLOCK = (s32)(SocketMsgFlag::Msg_DontWait);

        while (true) { // keep handling messages until would block
            // recv ws frame header+excess if available
            s32 recvSize = nn::socket::Recv(clientSocketFd, frameType__sizeByte__12more, 14, NOBLOCK);
            if (recvSize == -1) { return; } // XXX wheres the errno so we can see EWOULDBLOCK? lets just assume its EWOULDBLOCK
            if (recvSize != 14) { return; }
            if (frameType__sizeByte__12more[0] != 0x81) { return; } // FIXME noob impl -- lets just hope browsers dont like to fragment

            // get size+mask from header
            u8 maskKey[4];
            u32 payloadLen = 0;
            u8 sizeByte = 0x7f & frameType__sizeByte__12more[1]; // remove mask bit
            if (sizeByte < 0x7e) {
                payloadLen = sizeByte;
                *(u32*)&maskKey = *(u32*)&(frameType__sizeByte__12more[2]);
                // payload at [6...13]
            } else if (sizeByte == 0x7e) {
                payloadLen = frameType__sizeByte__12more[2] * 0x100 + frameType__sizeByte__12more[3];
                *(u32*)&maskKey = *(u32*)&(frameType__sizeByte__12more[4]);
                // payload at [8...13]
            } else if (sizeByte == 0x7f) {
                // bytes 2345 ignored, really cant imagine anyone needed 64b lengths lmao
                payloadLen = frameType__sizeByte__12more[6]*0x1000000 + frameType__sizeByte__12more[7]*0x10000 + frameType__sizeByte__12more[8]*0x100 + frameType__sizeByte__12more[9];
                *(u32*)&maskKey = *(u32*)&(frameType__sizeByte__12more[10]);
                // need to recv entire payload
            }

            // alloc payload, backfill excess header read bytes, recv remainder of payload -- this could block but we know its inflight
            u8 payload[payloadLen+1];
            if (sizeByte < 0x7e) {
                *(u64*)payload = *(u64*)(&frameType__sizeByte__12more[6]);
                recvSize = nn::socket::Recv(clientSocketFd, payload+8, payloadLen-8, 0); // blocking
                // assert recvSize == payloadLen-8
            } else if (sizeByte == 0x7e) {
                payload[0] = frameType__sizeByte__12more[8];
                payload[1] = frameType__sizeByte__12more[9];
                payload[2] = frameType__sizeByte__12more[10];
                payload[3] = frameType__sizeByte__12more[11];
                payload[4] = frameType__sizeByte__12more[12];
                payload[5] = frameType__sizeByte__12more[13];
                recvSize = nn::socket::Recv(clientSocketFd, payload+6, payloadLen-6, 0); // blocking
                // assert recvSize == payloadLen-6
            } else if (sizeByte == 0x7f) {
                recvSize = nn::socket::Recv(clientSocketFd, payload, payloadLen, 0); // blocking
                // assert recvSize == payloadLen
            }

            // unmask+terminate
            // TODO branch here on frameType is all we need for binary frames? just ignore excess string termination byte, it wont matter
            u32 payload_i = payloadLen;
            while (payload_i--) { payload[payload_i] ^= maskKey[payload_i%4]; }
            payload[payloadLen] = '\0';

            lotuskit::server::JsonDispatch::dispatchText((char*)payload);
        }
    }

    void WebSocket::listenAndWaitForFrontend(const char* bindIp, const u16 bindPort) {
        constexpr size_t MAX_HTTP_REQ_HEADER_LEN = 0x800;
        char buf[MAX_HTTP_REQ_HEADER_LEN];

        constexpr auto AF_INET = 2; // domain ams::socket::Family::Af_Inet (ipv4)
        if ((serverSocketFd = nn::socket::Socket(AF_INET, (s32)(SocketType::SOCK_STREAM), (s32)(SocketProtocol::IPPROTO_TCP))) < 0) {
            nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] socket::Socket() errno %d", serverSocketFd);
            svcOutputDebugString(buf, strlen(buf));
        }
        // set keepalive
        u32 flags = 1; // XXX what is this
        nn::socket::SetSockOpt(serverSocketFd, (s32)(SocketLevel::Sol_Socket), (u32)(SocketOption::So_KeepAlive), &flags, sizeof(flags));

        serverAddress.family = AF_INET;
        serverAddress.port = nn::socket::InetHtons(bindPort);
        nn::socket::InetAton(bindIp, (nn::socket::InAddr*)&serverAddress.address);

        s32 sockErrno;
        if ((sockErrno = nn::socket::Bind(serverSocketFd, &serverAddress, sizeof(serverAddress))) != 0) {
            nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] socket::Bind() errno %d", sockErrno);
            svcOutputDebugString(buf, strlen(buf));
            return;
        }

        isWsEstablished = false;
        clientSocketFd = 0;
        while (!isWsEstablished) {
            while (!clientSocketFd) {
                if ((sockErrno = nn::socket::Listen(serverSocketFd, 1)) != 0) {
                    nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] socket::Listen() errno %d", sockErrno);
                    svcOutputDebugString(buf, strlen(buf));
                    return;
                }

                nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] socket::Accept() for ws client on ws://%s:%d...", bindIp, bindPort);
                svcOutputDebugString(buf, strlen(buf));
                if ((clientSocketFd = nn::socket::Accept(serverSocketFd, &clientAddress, &clientAddressLen)) < 0) { // blocking
                    nn::util::SNPrintf(buf, sizeof(buf), "socket::Accept() errno %d", clientSocketFd);
                    svcOutputDebugString(buf, strlen(buf));
                    return;
                }
            }

            while (!isWsEstablished) {
                // TODO allow cors upon OPTIONS?
                // recv and upgrade+establish ws for GET
                s32 recvSize = nn::socket::Recv(clientSocketFd, buf, sizeof(buf), 0); // blocking

                if (buf[0] != 'G' || buf[1] != 'E' || buf[2] != 'T' || buf[3] != ' ' || buf[4] != '/' || buf[5] != ' ') {
                    svcOutputDebugString(buf, recvSize);
                    nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] closing client socket: no route found for req ^");
                    svcOutputDebugString(buf, strlen(buf));
                    nn::socket::Close(clientSocketFd);
                    clientSocketFd = 0;
                    break;
                }

                // Matched route: GET /
                char wsKey[33] = ""; wsKey[0] = '\0';
                bool isNewline = false;
                s32 buf_i = 0;
                do {
                    if (buf[buf_i] == '\r' && buf[buf_i+1] == '\n') {
                        isNewline = true;
                        buf_i += 2;
                    }

                    if (isNewline && // is Sec-WebSocket-Key header?
                        buf[buf_i+0] == 'S' &&
                        buf[buf_i+1] == 'e' &&
                        buf[buf_i+2] == 'c' &&
                        buf[buf_i+3] == '-' &&
                        buf[buf_i+4] == 'W' &&
                        buf[buf_i+5] == 'e' &&
                        buf[buf_i+6] == 'b' &&
                        buf[buf_i+7] == 'S' &&
                        buf[buf_i+8] == 'o' &&
                        buf[buf_i+9] == 'c' &&
                        buf[buf_i+10] == 'k' &&
                        buf[buf_i+11] == 'e' &&
                        buf[buf_i+12] == 't' &&
                        buf[buf_i+13] == '-' &&
                        buf[buf_i+14] == 'K' &&
                        buf[buf_i+15] == 'e' &&
                        buf[buf_i+16] == 'y' &&
                        buf[buf_i+17] == ':' &&
                        buf[buf_i+18] == ' '
                    ) {
                        buf_i += 19;
                        u8 wsKey_i = 0;
                        do { // copy the key string
                            if (buf[buf_i] == '\r') {
                                wsKey[wsKey_i] = '\0';
                                buf_i = recvSize-1; // break 2
                                break;
                            }
                            wsKey[wsKey_i] = buf[buf_i];
                        } while (++wsKey_i < 31 && ++buf_i < recvSize);
                    }
                    isNewline = false;
                } while(++buf_i < recvSize-2);
                wsKey[32] = '\0';

                if (wsKey[0] == '\0') {
                    svcOutputDebugString(buf, recvSize);
                    nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] closing client socket: Sec-WebSocket-Key header not found in GET / ^");
                    svcOutputDebugString(buf, strlen(buf));
                    nn::socket::Close(clientSocketFd);
                    clientSocketFd = 0;
                    break;
                }

                //nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] got ws key: %s ", wsKey);
                //svcOutputDebugString(buf, strlen(buf));

                // https://datatracker.ietf.org/doc/html/rfc6455#section-4.2.2
                // py acceptKey = base64.b64encode(hashlib.sha1(wsKey + WS_MAGIC).digest())
                const char* WS_MAGIC_FMT = "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; // 36 length
                const char* WS_RESP_FMT = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n";
                nn::util::SNPrintf(buf, sizeof(buf), WS_MAGIC_FMT, wsKey);
                u8 digest[20];
                websocketpp::sha1::calc(buf, strlen(buf), digest);
                auto acceptKey = websocketpp::base64_encode(digest, 20); // once this is sent, we may send() freely
                nn::util::SNPrintf(buf, sizeof(buf), WS_RESP_FMT, acceptKey.c_str());
                //svcOutputDebugString(buf, strlen(buf));
                /*s32 sentBytes =*/ nn::socket::Send(clientSocketFd, buf, strlen(buf), 0); // blocking

                nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] ws connection established");
                svcOutputDebugString(buf, strlen(buf));
                isWsEstablished = true;
            }
        }
    }

    void WebSocket::init() {
        socketPool = (u8*)stolenHeap->alloc(SOCKET_POOL_SIZE + ALLOCATOR_POOL_SIZE + PAGE_SIZE);
        socketPool = (u8*)ALIGN_UP(socketPool, PAGE_SIZE); // FIXME cant free this once we throw away alloc ptr

        WebSocketImpl::SendQueue::init();
        nn::nifm::Initialize();
        nn::socket::Initialize(socketPool, SOCKET_POOL_SIZE, ALLOCATOR_POOL_SIZE, 14);
        nn::nifm::SubmitNetworkRequest();
        while (nn::nifm::IsNetworkRequestOnHold()) {}
        if (!nn::nifm::IsNetworkAvailable()) {
            char buf[100];
            nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] nifm::IsNetworkAvailable() falsy");
            svcOutputDebugString(buf, strlen(buf));
            // XXX just ignore it? we'll see...
        }

        const char* bindIp = "0.0.0.0";
        const u16 bindPort = 7072;
        listenAndWaitForFrontend(bindIp, bindPort); // blocking

#if WEBSOCKET_DO_THREADED_SEND
        // create writer thread for this ws connection
        constexpr auto PRIO = 55;
        constexpr auto CORE_ID = 2;
        Handle sendHandle;
        svcCreateThread(&sendHandle, (void*)&ThreadFuncSend, nullptr, sendThreadStack+STACK_SIZE, PRIO, CORE_ID);
        svcStartThread(sendHandle);
#endif

    }

    void WebSocket::calc() {
        if (isWsEstablished) {
            recvNoblockAndProc(); // noblock poll ws -> dispatch commands (sometimes blocking)
            flushSendQueueBlocking(); // send any deferred logs since last proc. try to minimize useless logs!
            // FIXME until we get threaded sends to work, logging may impact performance/accuracy/???
            // TODO run some tests to verify ^ this is a real issue, use devtools throttling to break it, spam it etc and see what really blocks
        }

        const bool isCombo = *(u64*)&(lotuskit::tas::Record::currentInput.buttons) == SERVER_LISTEN_COMBO;
        if ((isCombo || isInternalReqListen) && listenReqDebounce == 0) {
            isInternalReqListen = false;
            listenReqDebounce = 15; // ignore button combo for n frames
            listenNotifDelay = 3; // defer server for n frames in order to display notice
        }
        if (listenReqDebounce > 0) { listenReqDebounce--; }
        if (listenNotifDelay > 0) {
            listenNotifDelay--;
            if (listenNotifDelay == 0) {
                if (!isWsEstablished) {
                    init(); // blocking
                } else {
                    // TODO clean disconnect? idk
                }
            } else {
                // display notice just before freezing the game
                lotuskit::TextWriter::appendCallback(2, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
                    textPos->x = 100;
                    textPos->y = 200;
                    writer->mScale.x = 3;
                    writer->mScale.y = 3;
                });
                lotuskit::TextWriter::printf(2, "[totk-lotuskit:%d] serving ws on 0.0.0.0:7072\n", TOTK_VERSION);
            }
        }
    }

} // ns
