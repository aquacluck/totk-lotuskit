#pragma once
#include "lib.hpp"

struct Counter {
    float mDeltaFrames;
    float mDeltaTime;
    int * mTimerMultiplierIndices;
    int mMaxTimerMultiplierCount;
    int mTimeMultiplierBaseIndex;
    int mTimeMultiplierIndex;
    undefined4 field6_0x1c;
    undefined8 field7_0x20;
};

struct TimeSpeedMultiplier {
    float mValue;
    float mTarget;
};

struct VFRMgr {
    float mRawDeltaFrame;
    float mRawDeltaTime;
    float mDeltaFrame;
    float mDeltaTime;
    struct Counter mPerCoreCounters[3];
    struct TimeSpeedMultiplier mTimeSpeedMultipliers[16];
    float mIntervalValue;
    u32 mCoreAndIntervalMask;
    //struct IDisposer mStaticDisposer;
};
