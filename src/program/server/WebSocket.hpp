#pragma once
#include "lib.hpp"
#include "lib/json.hpp"
//#include "proc/ModProc.hpp"
using json = nlohmann::json;

#define WEBSOCKET_DO_THREADED_SEND false
#define WEBSOCKET_DO_THREADED_RECV false

namespace lotuskit::server {
    class WebSocket {
        public:
        // Many things invokable (like this?) should have a name/repr used to: inject configs (romfs or otherwise), runtime wiring for scripting+syms, etc. Make sure it interops with codegen approaches!
        // TODO crazy things like https://stackoverflow.com/questions/35941045 ?
        inline static const json::json_pointer EXECNS = json::json_pointer("/server/WebSocket");
        static void calc();

        static void sendTextNoblock(const char*);
        static void sendTextBlocking(const char*);

        private:
        static void init();
        static void listenAndWaitForFrontend(const char* bindIp, const u16 bindPort);
        static void recvNoblockAndProc();
        static void flushSendQueueBlocking();
    };

} // ns
