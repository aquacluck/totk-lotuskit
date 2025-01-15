#pragma once
#include "lib.hpp"

#define WEBSOCKET_DO_THREADED_SEND false
#define WEBSOCKET_DO_THREADED_RECV false

namespace lotuskit::server {
    class WebSocket {
        public:
        static void init();
        static void calc();

        static void sendTextNoblock(const char*);
        static void sendTextBlocking(const char*);

        private:
        static void listenAndWaitForFrontend(const char* bindIp, const u16 bindPort);
        static void recvNoblockAndProc();
        static void flushSendQueueBlocking();
    };

} // ns
