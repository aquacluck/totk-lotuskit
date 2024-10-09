#pragma once

#include <nn/types.h>
#include <nn/util/MathTypes.h>

namespace nn::vfx2 {

class Emitter;

namespace detail {

struct ParticleData {
    int mIndex;
    int mCollisionCount;
    float mStartTime;
    float mLifeSpan;
    char _10[0x10];
};

struct ParentParticleData {
    Emitter* mEmitter;
    char _08[0x7c];
    float mLifeSpan;
    util::FloatRowMajor4x4 _88;
    util::FloatRowMajor4x4 _C8;
    util::FloatRowMajor4x4 _108;
    u8 _148[8];
    char _150[8];
};

struct ParticleProperty {
    util::FloatRowMajor4x4* mPosition; // component 4 is lifespan
    util::FloatRowMajor4x4* mVelocity; // component 4 is time
    util::FloatRowMajor4x4* mPrevVelocity; // component 4 is a random float
    util::FloatRowMajor4x4* mScale;
    util::FloatRowMajor4x4* mTimeScale;
    util::FloatRowMajor4x4* mRotation; // component 4 is the particle count
    util::FloatRowMajor4x4* mColor0;
    util::FloatRowMajor4x4* mColor1;
    util::FloatRowMajor4x4* _40;
    util::FloatRowMajor4x4* _48;
    util::FloatRowMajor4x4* _50;
    util::FloatRowMajor4x4* _58;
};

} // namespace detail

} // namespace nn::vfx2