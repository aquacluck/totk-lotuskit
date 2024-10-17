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
        static void Calc();

        static void SendTextNoblock(const char*);
        static void SendTextBlocking(const char*);

        private:
        static void Init();
        static void ListenAndWaitForFrontend(const char* bindIp, const u16 bindPort);
        static void RecvNoblockAndProc();
        static void FlushSendQueueBlocking();
    };

} // ns
