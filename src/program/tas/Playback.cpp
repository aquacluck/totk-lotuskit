#include "tas/Playback.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

namespace lotuskit::tas {
    void Playback::calc() {
        if (currentCtx == nullptr || currentCtxErrno != AngelScript::asEXECUTION_SUSPENDED) {
            return; // not doing playback
        }

        // TODO get delta time -> apply current input or resume ctx
        Logger::logText("TODO tas playback", "/tas/Playback");
    }
}
