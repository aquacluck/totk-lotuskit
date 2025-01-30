#pragma once
#include "exlaunch.hpp"
#include "tas/config.hpp"
#include <angelscript.h>
#include <nn/hid.h>

namespace lotuskit::tas {
    struct PlaybackInput {
        // 24B payload TODO gyro, merge with RecordInput?
        nn::hid::NpadButtonSet buttons; // u64
        nn::hid::AnalogStickState LStick; // s32 s32
        nn::hid::AnalogStickState RStick; // s32 s32
    };

    class Playback {
        public:
        static void calc();

        // script scheduling
        inline static AngelScript::asIScriptContext* currentCtx = nullptr; // if state asEXECUTION_SUSPENDED, will be resumed when needed to produce next input
        inline static u32 currentInputTTL60 = 0; // remaining 30fps half-frames to apply scheduled TAS input
        inline static u32 elapsedPlayback60 = 0; // 30fps half-frames elapsed this script execution (based on counting)

        // current input
        static PlaybackInput currentInput;
        static inline bool isPlaybackActive = false;
        static void setCurrentInput(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setCurrentInputOr(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setCurrentInputXor(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setSleepInput(u32 duration=1);

        inline static void applyCurrentInput(nn::hid::NpadBaseState* dst) {
            if (!isPlaybackActive) { return; }
            switch(config::playbackInputPassthroughMode) {
                case config::PlaybackInputPassthroughMode::NULL_VANILLA:
                // passthrough all, do not alter input ("sleep")
                break;

                case config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY:
                std::memcpy((void*)&(dst->mButtons), (void*)&(currentInput.buttons), 24);
                break;

                case config::PlaybackInputPassthroughMode::PASSTHROUGH_OR:
                *((u64*)&(dst->mButtons)) |= *((u64*)&(currentInput.buttons));
                // passthrough all non-zero axes
                dst->mAnalogStickL.mX = (dst->mAnalogStickL.mX != 0) ? dst->mAnalogStickL.mX : currentInput.LStick.mX;
                dst->mAnalogStickL.mY = (dst->mAnalogStickL.mY != 0) ? dst->mAnalogStickL.mY : currentInput.LStick.mY;
                dst->mAnalogStickR.mX = (dst->mAnalogStickR.mX != 0) ? dst->mAnalogStickR.mX : currentInput.RStick.mX;
                dst->mAnalogStickR.mY = (dst->mAnalogStickR.mY != 0) ? dst->mAnalogStickR.mY : currentInput.RStick.mY;
                // FIXME axis button flags not updated
                break;

                case config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR:
                *((u64*)&(dst->mButtons)) ^= *((u64*)&(currentInput.buttons));
                // toggle all non-zero axes
                dst->mAnalogStickL.mX = (currentInput.LStick.mX == 0) ? dst->mAnalogStickL.mX :
                                        ( dst->mAnalogStickL.mX == 0) ? currentInput.LStick.mX : 0;
                dst->mAnalogStickL.mY = (currentInput.LStick.mY == 0) ? dst->mAnalogStickL.mY :
                                        ( dst->mAnalogStickL.mY == 0) ? currentInput.LStick.mY : 0;
                dst->mAnalogStickR.mX = (currentInput.RStick.mX == 0) ? dst->mAnalogStickR.mX :
                                        ( dst->mAnalogStickR.mX == 0) ? currentInput.RStick.mX : 0;
                dst->mAnalogStickR.mY = (currentInput.RStick.mY == 0) ? dst->mAnalogStickR.mY :
                                        ( dst->mAnalogStickR.mY == 0) ? currentInput.RStick.mY : 0;
                // FIXME axis button flags not updated
                break;
            }
        }

        // utility
        static u32 duration60ToUIFrames(u32 duration60);
    };
} // ns
