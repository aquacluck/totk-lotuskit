#pragma once
#include "angelscript.h"
#include "structs/engineActor.hpp"

namespace lotuskit::script::globals {
    namespace ResidentActors {
        extern ::engine::actor::ActorBase* Player;
        extern ::engine::actor::ActorBase* PlayerCamera;
        extern ::engine::actor::ActorBase* EventCamera;
    }

    namespace sys {
        u64 mainOffset();
        u32 totkVersion();
        void heapInfo();
        void hookLimits();
        void memSearch(u64 needle);
        void threadInfo();
        void suspendCtx();
    } // ns

    // ns Logger
    //void trashPrint(auto &msg) {
    //    const char* str = msg.c_str();
    //    svcOutputDebugString(str, strlen(str));
    //}
    //void trashPrintInt(int num) {
    //    Logger::logJson(json::object({ {"oink", num} }), "/script/trashPrintInt");
    //}

    void registerGlobals(AngelScript::asIScriptEngine* engine);

} // ns
