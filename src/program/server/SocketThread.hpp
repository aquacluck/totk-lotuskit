#pragma once
#include "lib.hpp"
#include "lib/base64.hpp"
#include "lib/sha1.hpp"
#include "nn/nifm.h"
#include "nn/socket.h"
#include "nn/util.h"

namespace lotuskit::server::SocketThread {
    constexpr inline auto AF_INET = 2; // XXX domain ams::socket::Family::Af_Inet (ipv4)
    constexpr size_t STACK_SIZE = 0x2000; // XXX idk
    alignas(PAGE_SIZE) u8 sendStack[STACK_SIZE];

    constexpr size_t SOCKET_POOL_SIZE = 0x600000;
    constexpr size_t ALLOCATOR_POOL_SIZE = 0x20000;
    u8 socketPool[SOCKET_POOL_SIZE + ALLOCATOR_POOL_SIZE] __attribute__((aligned(0x4000)));
    s32 serverSocketFd;
    s32 clientSocketFd;
    sockaddr serverAddress;
    sockaddr clientAddress;
    u32 clientAddressLen;
    bool isWsEstablished = false;

    // sends queued/deferred ws messages. See SendBlock to bypass this and send immediately, eg to get a log out before a known crash
    void ThreadFuncSend(void* _) {
        char buf[200];
        while (true) {
            nn::util::SNPrintf(buf, sizeof(buf), "woo %d", 420);
            svcOutputDebugString(buf, strlen(buf));
            svcSleepThread(1000000000); // 1B ns = 1s
            //nn::os::SleepThread(nn::os::ConvertToTimeSpan(1000000000));
        }
    }

    void RecvNoblockAndProc() {
        if (!isWsEstablished) { return; }
    }

    void SendBlock() {
        if (!isWsEstablished) { return; }
    }

    void CreateAndWaitForFrontend() {
        constexpr size_t MAX_HTTP_REQ_HEADER_LEN = 0x800;
        char buf[MAX_HTTP_REQ_HEADER_LEN];

        // setup needs to happen in main thread
        nn::nifm::Initialize();
        nn::socket::Initialize(socketPool, SOCKET_POOL_SIZE, ALLOCATOR_POOL_SIZE, 14);
        nn::nifm::SubmitNetworkRequest();
        while (nn::nifm::IsNetworkRequestOnHold()) {}
        if (!nn::nifm::IsNetworkAvailable()) {
            //mState = LoggerState::UNAVAILABLE;
            nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] nifm::IsNetworkAvailable() falsy");
            svcOutputDebugString(buf, strlen(buf));
            // XXX just ignore it? we'll see...
        }
        if ((serverSocketFd = nn::socket::Socket(AF_INET, (s32)(SocketType::SOCK_STREAM), (s32)(SocketProtocol::IPPROTO_TCP))) < 0) {
            //mState = LoggerState::UNAVAILABLE;
            nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] socket::Socket() errno %d", serverSocketFd);
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
            nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] socket::Bind() errno %d", sockErrno);
            svcOutputDebugString(buf, strlen(buf));
            return;
        }

        isWsEstablished = false;
        clientSocketFd = 0;
        while (!isWsEstablished) {
            while (!clientSocketFd) {
                if ((sockErrno = nn::socket::Listen(serverSocketFd, 1)) != 0) {
                    nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] socket::Listen() errno %d", sockErrno);
                    svcOutputDebugString(buf, strlen(buf));
                    return;
                }

                nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] socket::Accept() for ws client on %d...", serverSocketFd);
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
                    nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] closing client socket: no route found for req ^");
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
                    nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] closing client socket: Sec-WebSocket-Key header not found in GET / ^");
                    svcOutputDebugString(buf, strlen(buf));
                    nn::socket::Close(clientSocketFd);
                    clientSocketFd = 0;
                    break;
                }

                //nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] got ws key: %s ", wsKey);
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

                nn::util::SNPrintf(buf, sizeof(buf), "[SocketThread] ws connection established");
                svcOutputDebugString(buf, strlen(buf));
                isWsEstablished = true;
            }
        }

        // create writer thread for this ws connection
        constexpr auto PRIO = 55;
        constexpr auto CORE_ID = 2;
        Handle sendHandle;
        svcCreateThread(&sendHandle, (void*)&ThreadFuncSend, nullptr, sendStack+STACK_SIZE, PRIO, CORE_ID);
        svcStartThread(sendHandle);
    }

} // ns
