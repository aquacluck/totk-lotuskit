#pragma once
#include "lib.hpp"
#include "angelscript.h"

namespace lotuskit::tas {
    class Playback {
        public:
        inline static AngelScript::asIScriptContext* currentCtx = nullptr;
        inline static s32 currentCtxErrno = 0;
        static void calc();
    };
} // ns
