#pragma once
#include "exlaunch.hpp"
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

        // current input
        static PlaybackInput currentInput;
        static inline bool isPlaybackActive = false;
        static inline bool isSleepInput = false; // passthrough human input
        static void setCurrentInput(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setSleepInput(u32 duration=1);
        inline static void applyCurrentInput(nn::hid::NpadBaseState* dst) {
            if (!isPlaybackActive || isSleepInput) { return; }
            std::memcpy((void*)&(dst->mButtons), (void*)&(currentInput.buttons), 24);
        }
    };
} // ns
