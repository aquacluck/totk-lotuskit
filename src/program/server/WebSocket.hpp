#pragma once
#include "lib.hpp"
#include "lib/base64.hpp"
#include "lib/sha1.hpp"
#include "nn/nifm.h"
#include "nn/socket.h"
#include "nn/util.h"

#define WEBSOCKET_DO_THREADED_SEND false
#define WEBSOCKET_DO_THREADED_RECV false

namespace lotuskit::server::WebSocket {
    namespace SendFrame {
        inline u8 HeaderLenFromPayloadLen(u32 pLen) {
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

        inline void SerializeHeader(u8* dst, u8 frameType, u8 hLen, u32 pLen) {
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

        inline u32 GetSerializedFrameSize(u8* frame) {
            if (frame[1] < 0x7e) { return 2 + frame[1]; }
            if (frame[1] == 0x7e) { return 4 + frame[2]*0x100 + frame[3]; }
            if (frame[1] == 0x7f) { return 10 + frame[6]*0x1000000 + frame[7]*0x10000 + frame[8]*0x100 + frame[9]; }
            return 0;
        }
    } // ns

    namespace SendQueue {
        nn::os::MutexType enqueueMutex;
        constexpr size_t SEND_QUEUE_SIZE = 0x100000;
        alignas(PAGE_SIZE) u8 sendQueueBuf[SEND_QUEUE_SIZE];
        const u8* sendQueueBufEnd = sendQueueBuf + SEND_QUEUE_SIZE - 1;
        u8* sendQueueHead = sendQueueBuf;
        u8* sendQueueTail = sendQueueBuf;

        void Setup() {
            sendQueueBuf[0] = 0x00;
            nn::os::InitializeMutex(&enqueueMutex, true, 0);
        }

        inline u8* GetNextFrameForEnqueue(u32 fLen) {
            if (sendQueueHead + fLen <= sendQueueBufEnd) {
                return sendQueueHead; // not near the end, it fits
            }
            // it won't fit at the end, wrap to beginning
            if (sendQueueBufEnd - sendQueueHead > 0) {
                *sendQueueHead = 0x00; // null frameType to indicate no more frames past this point
            }
            return sendQueueBuf;
        }

        inline void EnqueueText(const char* payload) {
            nn::os::LockMutex(&enqueueMutex);

            u32 pLen = strlen(payload);
            u8 hLen = SendFrame::HeaderLenFromPayloadLen(pLen);
            u32 fLen = hLen + pLen; // frame is header + payload

            u8* dst = GetNextFrameForEnqueue(fLen);
            SendFrame::SerializeHeader(dst, 0x81, hLen, pLen);
            std::memcpy(&dst[hLen], payload, pLen);

            sendQueueHead = (u8*)ALIGN_UP(dst + fLen, 8); // advance ptr to next
            if (sendQueueHead >= sendQueueBufEnd) {
                sendQueueHead = sendQueueBuf; // wrap to beginning
            }

            nn::os::UnlockMutex(&enqueueMutex);
        }

        inline void DequeueFrame(u32 fLen) {
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

    constexpr size_t SOCKET_POOL_SIZE = 0x100000;
    constexpr size_t ALLOCATOR_POOL_SIZE = 0x20000;
    u8 socketPool[SOCKET_POOL_SIZE + ALLOCATOR_POOL_SIZE] __attribute__((aligned(0x4000)));
    s32 serverSocketFd;
    s32 clientSocketFd;
    sockaddr serverAddress;
    sockaddr clientAddress;
    u32 clientAddressLen;
    bool isWsEstablished = false;

#if WEBSOCKET_DO_THREADED_SEND
    constexpr size_t STACK_SIZE = 0x2000; // XXX idk
    alignas(PAGE_SIZE) u8 sendThreadStack[STACK_SIZE];

    // sends queued/deferred ws messages. See SendTextBlocking to bypass this and send immediately, eg to get a log out before a known crash
    void ThreadFuncSend(void* _) {
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

    // alternative to ThreadFuncSend, flushing deferred messages
    void FlushSendQueueBlocking() {
        if (!isWsEstablished) { return; }
        while (SendQueue::sendQueueTail != SendQueue::sendQueueHead) {
            u32 fLen = SendFrame::GetSerializedFrameSize(SendQueue::sendQueueTail);
            nn::socket::Send(clientSocketFd, SendQueue::sendQueueTail, fLen, 0); // blocking
            SendQueue::DequeueFrame(fLen);
        }
    }

    void SendTextNoblock(const char* payload) {
        if (!isWsEstablished) { return; }
        SendQueue::EnqueueText(payload); // (it can still block on the mutex, could prob be lock free if head is atomic ptr we bump by frame size then fill behind it)
    }

    void SendTextBlocking(const char* payload) {
        if (!isWsEstablished) { return; }
        // TODO fd mutex?

        // plaintext json only. its tempting to use some fancy binary format but:
        // - i like being able to read the wire in devtools network tab
        // - interop
        // - size difference isnt meaningful at reasonable msg volumes
        // - i dont care about decode speed in the mod, busy workloads should be builtin or script driven
        // - for hot logging paths, manually encoding simple json will be faster than a lib anyways, and textwriter or files may be more appropriate
        // - in the browser, plain json is usually just as fast as binary formats. this one is asmjs tho https://github.com/artcompiler/L16/blob/master/src/ubjson.js
        // so lets not deal with it unless we really need it
        u32 pLen = strlen(payload);
        u8 hLen = SendFrame::HeaderLenFromPayloadLen(pLen);
        u32 fLen = hLen + pLen; // frame is header + payload
        char buf[fLen];
        SendFrame::SerializeHeader((u8*)buf, 0x81, hLen, pLen);
        std::memcpy(&buf[hLen], payload, pLen);
        nn::socket::Send(clientSocketFd, buf, fLen, 0); // blocking
    }

    void RecvNoblockAndProc() {
        if (!isWsEstablished) { return; }
    }

    void CreateAndWaitForFrontend() {
        constexpr size_t MAX_HTTP_REQ_HEADER_LEN = 0x800;
        char buf[MAX_HTTP_REQ_HEADER_LEN];

        // setup needs to happen in main thread
        SendQueue::Setup();
        nn::nifm::Initialize();
        nn::socket::Initialize(socketPool, SOCKET_POOL_SIZE, ALLOCATOR_POOL_SIZE, 14);
        nn::nifm::SubmitNetworkRequest();
        while (nn::nifm::IsNetworkRequestOnHold()) {}
        if (!nn::nifm::IsNetworkAvailable()) {
            //mState = LoggerState::UNAVAILABLE;
            nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] nifm::IsNetworkAvailable() falsy");
            svcOutputDebugString(buf, strlen(buf));
            // XXX just ignore it? we'll see...
        }
        constexpr auto AF_INET = 2; // XXX domain ams::socket::Family::Af_Inet (ipv4)
        if ((serverSocketFd = nn::socket::Socket(AF_INET, (s32)(SocketType::SOCK_STREAM), (s32)(SocketProtocol::IPPROTO_TCP))) < 0) {
            //mState = LoggerState::UNAVAILABLE;
            nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] socket::Socket() errno %d", serverSocketFd);
            svcOutputDebugString(buf, strlen(buf));
        }
        // set keepalive
        u32 flags = 1; // XXX what is this
        nn::socket::SetSockOpt(serverSocketFd, (s32)(SocketLevel::Sol_Socket), (u32)(SocketOption::So_KeepAlive), &flags, sizeof(flags));

        const u16 bindPort = 7072;
        serverAddress.family = AF_INET;
        serverAddress.port = nn::socket::InetHtons(bindPort);
        nn::socket::InetAton("127.0.0.1", (nn::socket::InAddr*)&serverAddress.address);

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

                nn::util::SNPrintf(buf, sizeof(buf), "[WebSocket] socket::Accept() for ws client on %d...", serverSocketFd);
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
                s32 recvSize = nn::socket::Recv(clientSocketFd, buf, sizeof(buf), 0);

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

#if WEBSOCKET_DO_THREADED_SEND
        // create writer thread for this ws connection
        constexpr auto PRIO = 55;
        constexpr auto CORE_ID = 2;
        Handle sendHandle;
        svcCreateThread(&sendHandle, (void*)&ThreadFuncSend, nullptr, sendThreadStack+STACK_SIZE, PRIO, CORE_ID);
        svcStartThread(sendHandle);
#endif
    }

} // ns
