#include <nn/util.h>
#include "tas/config.hpp"
#include "tas/Playback.hpp"
#include "structs/VFRMgr.hpp"
#include "Logger.hpp"
#include "TextWriter.hpp"
using Logger = lotuskit::Logger;

namespace lotuskit::tas {
    PlaybackInput Playback::currentInput = {0};

    void Playback::calc() {
        if (!isPlaybackActive) { return; } // not doing playback

        // TODO elapsed frame count? system tick? this frame's delta time? rta? igt?
        // (however its impossible to predict or progress-bar a script's frame duration -- halting problem)
        lotuskit::TextWriter::printf(1, "[tas::Playback] TOTK_%d\n", TOTK_VERSION);
        VFRMgr* vfrMgr = *EXL_SYM_RESOLVE<VFRMgr**>("engine::module::VFRMgr::sInstance");

        // int 2 or 3 = float 1.0 (@30fps) or 1.5 (@20fps)
        // assert(mDeltaFrame == 1.0 || mDeltaFrame == 1.5); // precisely
        const u32 deltaFrame60 = (u32)(vfrMgr->mDeltaFrame * 2);

        // should the current input state still be scheduled?
        if (currentInputTTL60 >= deltaFrame60) {
            // deduct frames from schedule
            currentInputTTL60 -= deltaFrame60;
        } else {
            // only 0/1/2 frames60 left on schedule and deltaFrame60 is larger
            // XXX is erring on one side or the other better, until i properly figure this out?
            //     ie playing out these closing frames and deferring or eating into subsequent inputs? sounds complicated in the dumb way
            currentInputTTL60 = 0; // abandon/truncate the remainder of this scheduled input and resume script
        }

        if (currentInputTTL60 > 0) {
            return; // keep using current scheduled input
        }

        if (currentCtx == nullptr || currentCtx->GetState() != AngelScript::asEXECUTION_SUSPENDED) {
            Logger::logText("[ERROR] tas script ctx missing, cannot advance input", "/tas/Playback");
            return; //isPlaybackActive = false; // XXX is it better to repeat the last input or drop out? this has never happened yet
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

    void Playback::setCurrentInput(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        // called by tas script to specify input for next n frames
        if (duration == 0) { return; } // ignore

        // what is "n frames" expected to mean?
        u32 duration60 = 0;
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            duration60 = duration;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            duration60 = duration * 2;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            duration60 = duration * 3;
        } else return;
        // assert duration60 > 0

        AngelScript::asIScriptContext *ctx = AngelScript::asGetActiveContext();
        if (ctx != nullptr) {
            //TODO atomic toggle between double buffer currentInput+nextInput
            currentInputTTL60 = duration60;
            currentInput.LStick.mX = nextLStickX;
            currentInput.LStick.mY = nextLStickY;
            currentInput.RStick.mX = nextRStickX;
            currentInput.RStick.mY = nextRStickY;

            // rehydrate axis flags, matching 16400 sdk threshold (arguably bad for us, hiding low signals). idk why these are buttons but it's useful to preserve them for InputDisplay
            if (nextLStickX >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLRight); }
            if (nextLStickX < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLLeft); }
            if (nextLStickY >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLUp); }
            if (nextLStickY < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLDown); }
            if (nextRStickX >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRRight); }
            if (nextRStickX < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRLeft); }
            if (nextRStickY >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRUp); }
            if (nextRStickY < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRDown); }

            *(u64*)&(currentInput.buttons) = nextButtons; // These are expected to be
            isPlaybackActive = true;                      // written/observable in order
            // XXX and i'm not confident that's guaranteed, but it doesnt seem dangerous at least?
            //     i do this sort of guard without locking a lot though, so i need to find out. std::memory_order or something?

            // yield execution from script back to game, to be resumed in n frames
            currentCtx = ctx;
            ctx->Suspend(); // assert ctx->GetState() == asEXECUTION_SUSPENDED
        }
    }
} // ns
