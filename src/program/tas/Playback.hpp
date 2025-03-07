#pragma once
#include <string>
#include "exlaunch.hpp"
#include "tas/config.hpp"
#include <angelscript.h>
#include <nn/hid.h>
#include "structs/nnSixAxis.hpp"

namespace lotuskit::tas {
    struct PlaybackInput {
        // 24B main input payload
        nn::hid::NpadButtonSet buttons; // u64
        nn::hid::AnalogStickState LStick; // s32 s32
        nn::hid::AnalogStickState RStick; // s32 s32
        // 0x60
        nn::hid::SixAxisSensorState gyro;
    };

    class Playback {
        public:
        static void calc();

        // script scheduling
        inline static AngelScript::asIScriptContext* currentCtx = nullptr; // if state asEXECUTION_SUSPENDED, will be resumed when needed to produce next input
        inline static u32 currentInputTTL60 = 0; // remaining 30fps half-frames to apply scheduled TAS input
        inline static u32 elapsedPlayback60 = 0; // 30fps half-frames elapsed this script execution (based on counting)

        // pause scheduling
        static bool calcScheduleIsAwaitPauseRequest();
        static void doScheduleAwaitPauseRequestStr(const std::string& requestKey);
        static void doScheduleAwaitUnpauseRequestStr(const std::string& requestKey);
        static bool calcScheduleIsAwaitPauseTarget();
        static void doScheduleAwaitPauseTargetStr(const std::string& targetKey);
        static void doScheduleAwaitUnpauseTargetStr(const std::string& targetKey);
        inline static bool skipLoadingPause = true; // frames with LoadingPause do not count toward input timing
        inline static void doSkipLoadingPause(bool v) { skipLoadingPause = v; }

        // current input
        static PlaybackInput currentInput;
        inline static bool isPlaybackActive = false;
        static void setCurrentInput(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setCurrentInputOr(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setCurrentInputXor(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
        static void setSleepInput(u32 duration=1);
        static void applyCurrentInput(nn::hid::NpadBaseState* dst);

        // current gyro input: these calls do not schedule frames! you must call tas::input(n) to begin+continue tas playback!
        static void applyCurrentGyro(nn::hid::SixAxisSensorState* dst_gyro);
        inline static void setCurrentGyroLinearAcceleration(const sead::Vector3f& linearAcceleration) { currentInput.gyro.linearAcceleration = linearAcceleration; }
        inline static void setCurrentGyroAngularVelocity(const sead::Vector3f& angularVelocity) {       currentInput.gyro.angularVelocity    = angularVelocity; }
        inline static void setCurrentGyroAngularVelocitySum(const sead::Vector3f& angularVelocitySum) { currentInput.gyro.angularVelocitySum = angularVelocitySum; }
        inline static void setCurrentGyroRotation(const sead::Matrix33f& rotation) {                    currentInput.gyro.rotation           = rotation; }
        inline static void setCurrentGyroAll(const sead::Vector3f& linearAcceleration, const sead::Vector3f& angularVelocity, const sead::Vector3f& angularVelocitySum, const sead::Matrix33f& rotation) {
            currentInput.gyro.linearAcceleration = linearAcceleration;
            currentInput.gyro.angularVelocity    = angularVelocity;
            currentInput.gyro.angularVelocitySum = angularVelocitySum;
            currentInput.gyro.rotation           = rotation;
        }
        inline static void setCurrentGyroAllZero() {
            auto v = sead::Vector3f{0, 0, 0};
            auto m = sead::Matrix33f{0,0,0, 0,0,0, 0,0,0};
            setCurrentGyroAll(v, v, v, m);
        }

        // utility
        static void drawTextWriterModeLine();
        static u32 duration60ToUIFrames(u32 duration60);
    };
} // ns
