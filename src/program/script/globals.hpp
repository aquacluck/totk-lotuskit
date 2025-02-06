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
        u64 tick();
        void heapInfo();
        void hookLimits();
        void memSearch(u64 needle);
        void threadInfo();
        void suspendCtx();
        void wsListen();
    } // ns

    void registerGlobals(AngelScript::asIScriptEngine* engine);

} // ns
