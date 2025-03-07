#pragma once
#include <math/seadMatrix.h>
#include <math/seadVector.h>

namespace nn::hid {
    class SixAxisSensorState {
        public:
        u64 deltaTime; // maybe always 5_000_000?
        u64 mSamplingNumber;

        // x  linear axis: left (positive) and right (negative)
        // x angular axis: pitching forward (negative) and backward (positive, tilting toward you like a bottle of water)
        // y  linear axis: from the Z buttons and usb port (negative) down to the grips (positive)
        // y angular axis: rolling clockwise (negative) and ccw (positive), like you're dangling it from the cable and spinning
        // z  linear axis: the face (negative, closer to your face) and rear (positive, holding it it farther away) of the controller
        // z angular axis: clockwise (negative) and ccw (positive) when the controller is flat on a surface

        // linear acceleration is in g-force units. At rest -1g will be reported along the axis of gravity. This is negative/inverted
        // because the gyro reports your table/hands/etc "accelerating it upward" to resist gravity, not the pull of gravity itself:
        // - eg with the controller set flat, z = -1 up toward analog sticks, not down. Flinging up will continue making z more negative,
        //   while flinging down will first cancel out gravity, and then exceed it making z positive if you force it downwards faster than gravity.
        sead::Vector3f linearAcceleration;
        sead::Vector3f angularVelocity; // XXX what unit?
        sead::Vector3f angularVelocitySum; // signed whole rotations of angularVelocity axis, integral of angularVelocity over time, does not update when angularVelocity is 0
        sead::Matrix33f rotation; // orientation of controller. This is more granular+responsive than angularVelocitySum and XXX probably used for most aiming

        u32 attributes; // 1 IsConnected, 2 IsInterpolated
        u32 reserved;
    };
    static_assert(sizeof(SixAxisSensorState) == 0x60);
}

