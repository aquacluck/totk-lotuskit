#pragma once
#include <string>
#include "exlaunch.hpp"
#include <angelscript.h>
#include <nn/hid.h>
#include "structs/nnSixAxis.hpp"

namespace lotuskit::tas::Playback {
    struct PlaybackInput {
        // 24B main input payload
        nn::hid::NpadButtonSet buttons; // u64
        nn::hid::AnalogStickState LStick; // s32 s32
        nn::hid::AnalogStickState RStick; // s32 s32
        // 0x60
        nn::hid::SixAxisSensorState gyro;
    };

    // entrypoint from mainloop + io for injection
    void calc(); // called each frame, may resume tas script to generate next input
    bool applyCurrentInput(nn::hid::NpadBaseState* dst); // true when dst mutated
    bool applyCurrentGyro(nn::hid::SixAxisSensorState* dst_gyro); // true when dst mutated

    // script binds for: pause scheduling
    void doScheduleAwaitPauseRequestStr(const std::string& requestKey);
    void doScheduleAwaitUnpauseRequestStr(const std::string& requestKey);
    void doScheduleAwaitPauseTargetStr(const std::string& targetKey);
    void doScheduleAwaitUnpauseTargetStr(const std::string& targetKey);
    void doBlockOnDebugPause(bool);
    void doBlockOnLoadingPause(bool);

    // script binds for: current input
    void setCurrentInput(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
    void setCurrentInputOr(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
    void setCurrentInputXor(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0);
    void setSleepInput(u32 duration=1);

    // script binds for: current gyro input: these calls do not schedule frames! you must call tas::input(n) to begin+continue tas playback!
    void setCurrentGyroLinearAcceleration(const sead::Vector3f& linearAcceleration);
    void setCurrentGyroAngularVelocity(const sead::Vector3f& angularVelocity);
    void setCurrentGyroAngularVelocitySum(const sead::Vector3f& angularVelocitySum);
    void setCurrentGyroRotation(const sead::Matrix33f& rotation);
    void setCurrentGyroAll(const sead::Vector3f& linearAcceleration, const sead::Vector3f& angularVelocity, const sead::Vector3f& angularVelocitySum, const sead::Matrix33f& rotation);
    void setCurrentGyroAllZero();

    // script binds for: stepping through an existing script (this sp = interrupt ctx with blockOnDebugPause, <sp = script to step through)
    void beginFrameAdvance();
    void stepFrameAdvance(u32 duration=1);
    void endFrameAdvance();

    // internal/utility
    void drawTextWriterModeLine();

} // ns

