#pragma once
#include "lib.hpp"
#include "lib/json.hpp"
//#include "proc/ModProc.hpp"
using json = nlohmann::json;

#define WEBSOCKET_DO_THREADED_SEND false
#define WEBSOCKET_DO_THREADED_RECV false

namespace lotuskit::server {
    namespace WebSocket {
        // Many things invokable (like this?) should have a name/repr used to: inject configs (romfs or otherwise), runtime wiring for scripting+syms, etc. Make sure it interops with codegen approaches!
        // TODO crazy things like https://stackoverflow.com/questions/35941045 ?
        // TODO note that there's no path/pointer here to reflect->execute CreateAndWaitForFrontend. We need that (whether a standard Proc class+method or something else) and wire scripting to invoke it.
        const json::json_pointer EXECNS("/server/WebSocket");

        void FlushSendQueueBlocking();
        void SendTextNoblock(const char*);
        void SendTextBlocking(const char*);
        void RecvNoblockAndProc();
        void CreateAndWaitForFrontend();

    } // ns
} // ns
