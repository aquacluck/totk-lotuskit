#include "nn/util.h"
#include "tas/Playback.hpp"
#include "syms_merged.hpp"
#include "structs/VFRMgr.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

namespace lotuskit::tas {
    PlaybackInput Playback::currentInput = {0};

    void Playback::calc() {
        if (!isPlaybackActive) { return; } // not doing playback

        // is the current input still playing?
        VFRMgr* vfrMgr = *exl::util::pointer_path::FollowSafe<VFRMgr*, sym::engine::module::VFRMgr::sInstance::offset>();
        // assert(mDeltaFrame == 1.0 or 1.5 @30fps)
        currentInputTTL60 -= (u32)(vfrMgr->mDeltaFrame * 2);
        if (currentInputTTL60 > 0) { return; } // keep using current input
        if (currentInputTTL60 < 0) { currentInputTTL60 = 0; } // bound this to 0 so it cant underflow then start consuming time when another script is loaded

        if (currentCtx == nullptr || currentCtx->GetState() != AngelScript::asEXECUTION_SUSPENDED) {
            Logger::logText("[ERROR] tas script ctx missing, cannot advance input", "/tas/Playback");
            //isPlaybackActive = false; // XXX is it better to repeat the last input or drop out?
            return;
        }

        // resume script, eventually getting a setCurrentInput call
        // XXX non-setCurrentInput yields will create input gaps during tas playback -- "pure" tas needs the inputs and can't be waiting around for other events.
        //     Experimental workflows can just manually resync/etc to work around this sort of thing.
        s32 asErrno = currentCtx->Execute();
        if (asErrno == AngelScript::asEXECUTION_FINISHED) {
            isPlaybackActive = false; // XXX ensure this doesnt drop the last input
            currentCtx->Release();
            currentCtx = nullptr;

        } else if (asErrno == AngelScript::asEXECUTION_EXCEPTION) {
            // TODO exceptions
            isPlaybackActive = false;
            char buf[1000];
            nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] uncaught: %s", currentCtx->GetExceptionString());
            Logger::logText(buf, "/script/engine"); // TODO extract s32 script::engine::resumeCtx(asIScriptContext*)?
            currentCtx->Release();
            currentCtx = nullptr;

        } else if (asErrno == AngelScript::asEXECUTION_SUSPENDED) {
            // do not release ongoing async ctx
        }
    }

    void Playback::setCurrentInput(u32 duration60, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        // called by tas script to specify input for next n frames
        if (duration60 == 0) { return; } // ignore

        AngelScript::asIScriptContext *ctx = AngelScript::asGetActiveContext();
        if (ctx != nullptr) {
            //TODO atomic toggle between double buffer currentInput+nextInput
            currentInputTTL60 = duration60;
            *(u64*)&(currentInput.buttons) = nextButtons;
            currentInput.LStick.mX = nextLStickX;
            currentInput.LStick.mY = nextLStickY;
            currentInput.RStick.mX = nextRStickX;
            currentInput.RStick.mY = nextRStickY;
            isPlaybackActive = true;

            // yield execution from script back to game, to be resumed in n frames
            currentCtx = ctx;
            ctx->Suspend(); // assert ctx->GetState() == asEXECUTION_SUSPENDED
        }
    }
} // ns
