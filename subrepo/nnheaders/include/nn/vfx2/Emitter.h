#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/os/os_Mutex.h>
#include <nn/util/MathTypes.h>
#include <nn/vfx2/detail/ParticleData.h>
#include <nn/vfx2/detail/Random.h>
#include <nn/nn_BitTypes.h>
#include <nn/types.h>

namespace nn::vfx2 {

namespace detail {
class BufferAllocator;
struct ResEmitter;
} // namespace detail

class Emitter;
class EmitterResource;
class EmitterSet;
class Heap;

struct ParentEmitterInfo {
    util::Float4 mScale;
    util::Float4 mRotation;
    util::Float4 mTimeScale;
    util::Float4 _30;
    util::Float4 _40;
    util::Float4 _50;
    util::Float4 _60;
    Emitter* mEmitter;
    int _78;
    int mChildCount;
    int mParticleIndex;
    int _84;
    float mLifeSpan;
    float mElapsedTime;
    float _90;
    char _94[0xc];
};

// TODO: think of a better name for this
struct EmitterConnections {
    EmitterResource* mResources[16];
    Emitter* mEmitters[16];
    void* _100[16];
    detail::ParentParticleData* mParentParticleData[16];
};

struct EmitterAnimValue {
    util::Float3 _00;
    util::Float3 mRotation;
    util::Float3 _18;
    util::Float3 mColor0;
    util::Float3 mColor1;
    util::Float3 mEmissionRate;
    util::Float3 mLifeScale;
    util::Float3 mAlpha0;
    util::Float3 mAlpha1;
    util::Float3 mObjectVelScale;
    util::Float3 mDirectionalVelScale;
    util::Float3 mScale;
    util::Float3 mSizeScale;
    util::Float3 _9C;
};

class Emitter {
public:
    struct Callbacks {
        void* mCapdCallback;
        void* _08;
        void* _10;
        void* _18;
        void* _20;
        void* _28;
        void* _30;
        void* mEmitterPluginUserData;
        void* _40;
        void* mFgwdCallback;
        void* _50;
        void* mFcsfCallback;
        void* _60;
    };

    struct InitializeArg {
        Callbacks* callbacks;
        gfx::TDevice<gfx::DefaultApi>* device;
        EmitterSet* emitter_set;
        EmitterResource* resource;
        Heap* heap;
        Emitter* parent;
        char _30[8];
        int _38;
        int _3C;
    };

    bool Initialize(InitializeArg&, void*);

private:
    EmitterSet* mEmitterSet;
    Emitter* mNext;
    char _10[0x18];
    float mElapsedTime;
    float mLifeSpan;
    Bit32 mFlags;
    Bit32 mKeyFrameMask;
    int _38[2];
    EmitterResource* mResource;
    detail::ResEmitter* mResEmitter;
    char _50[8];
    EmitterConnections* mEmitterConnections;
    ParentEmitterInfo* mParentInfo;
    detail::ParticleData* mParticles;
    detail::ParticleProperty* mParticleProperties;
    u32 _78;
    float _7C;
    float mAlpha0;
    float mAlpha1;
    u8 mCurrentParticlePropertyIndex;
    u8 _89;
    char _8A[14];
    int mParticleCount;
    int mEmitGroupCount;
    int mRequiredParticleAssignmentCount;
    int mParticleIndexIter;
    float mEmitStartFrame;
    float _AC;
    util::Float4 _B0;
    util::Float4 _C0;
    int _D0;
    int mEmittedCount;
    float _D8;
    float mEmittedAmount; // in my notes but I forgot what this meant
    float mEmissionInterval;
    float mEmissionRate;
    float mEmissionIntervalScale;
    float mLifeScale;
    int _F0[2];
    int mEmitterSetIndex;
    int _FC;
    util::FloatRowMajor4x4 _100[6];
    char _280[0x80];
    detail::ParticleProperty* mParticlePropertySets;
    int _308;
    detail::ParticleProperty mParticleProperty;
    Callbacks mCallbacks;
    EmitterAnimValue mAnimValues;
    char _480[0x20];
    detail::Random mRandom;
    char _4A8[0x10];
    Emitter* _4B8;
    char _4C0[0x170];
    u8 _630;
    char _631[0x27];
    detail::BufferAllocator* mBufferAllocator;
    char _660[0x70];
    os::Mutex mHeapMutex;
    Heap* mHeap;
    size_t mAllocSize;
    u64 mAllocCount;
    u8 mMaxParticleCount;
    char _709[0x37];
};

} // namespace nn::vfx2