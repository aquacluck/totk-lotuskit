#pragma once
#include "exlaunch.hpp"
#include <heap/seadHeap.h>

#define WEBSOCKET_DO_THREADED_SEND false
#define WEBSOCKET_DO_THREADED_RECV false

namespace lotuskit::server {
    class WebSocket {
        public:
        static void init();
        static void calc();

        static void sendTextNoblock(const char*);
        static void sendTextBlocking(const char*);

        inline static sead::Heap* stolenHeap = nullptr;
        inline static void assignHeap(sead::Heap* heap) {
            // assert only called once
            stolenHeap = heap;
        }

        inline static bool isInternalReqListen = false; // set to request the server to listen

        private:
        static void listenAndWaitForFrontend(const char* bindIp, const u16 bindPort);
        static void recvNoblockAndProc();
        static void flushSendQueueBlocking();
    };

} // ns
