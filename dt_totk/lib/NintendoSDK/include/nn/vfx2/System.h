/**
 * @file System.h
 * @brief VFX system implementation.
 */

#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>
#include <nn/vfx2/detail/BufferAllocator.h>
#include <nn/vfx2/detail/Mutex.h>
#include <nn/vfx2/detail/StripeAllocator.h>
#include <nn/vfx2/detail/StripeSystem.h>
#include <nn/vfx2/detail/SuperStripeSystem.h>
#include <nn/vfx2/Config.h>
#include <nn/vfx2/Heap.h>
#include "nn/vfx2/Resource.h"

namespace nn::vfx2 {

namespace detail {
class ConnectionStripeSystem;
class EmitterCalculator;
class Shader;
class TextureSamplerManager;
} // namespace detail

class Emitter;
class EmitterSet;
class TemporaryBuffer;

struct EmitterSetArray {
    EmitterSet* mArray;
    int mCount;
    char _0C[0x14];
};

struct CallbackStruct {
    void* _00;
    char _08[0x48];
    void* _50;
    char _58[0x10];
};

struct ResourceArray {
    Resource** mResources;
    CallbackStruct* mCallbackStruct;
    char _10[8];
    void* mRegisterTextureCallback;
};

class System {
public:
    explicit System(Config const&);

    virtual ~System();
    virtual void Initialize(Heap*, Heap*, Config const&);

private:
    using FindShader = void (*)(void*, detail::Shader*&);

    char _08[0x20];
    size_t* mTempBufferSizes;
    detail::TextureSamplerManager* _30;
    size_t mDefaultTempBufferSize;
    char _40[8];
    int mEmitterCount;
    int mEmitterSetCount;
    char _50[0x1e];
    bool mSupressLogOutput;
    bool mInitRandom;
    char _70[0x28];
    TemporaryBuffer* mTemporaryBuffers;
    char _A0[8];
    gfx::Device* mDevice;
    char _B0[8];
    EmitterSet* mActiveEmitterSets;
    char _C0[0x1f8];
    EmitterSet* _2B8;
    char _2C0[0x1f8];
    EmitterSet* _4B8;
    char _4C0[0x1f8];
    EmitterSet* _6B8;
    char _6C0[0x1f8];
    detail::EmitterCalculator* mEmitterCalculator;
    char _8C0[0x10];
    void* mBufferAllocatorMemory;
    void* mMemoryPoolMemory;
    char _8E0[0x10];
    detail::TextureSamplerManager* mTextureSamplerManager;
    detail::TextureSamplerManager* _8F8;
    char _900[0x98];
    ResHeap mResStaticHeap;
    char _9E0[8];
    detail::Mutex mDynamicHeapMutex;
    Heap* mDynamicHeap;
    size_t mDynamicHeapAllocSize;
    u64 mDynamicHeapAllocCount;
    EmitterSetArray mEmitterSets;
    char _A48[0x1e8];
    int _C30;
    EmitterSet** mFreeEmitterSets;
    u64 mFreeEmitterSetCount;
    char _C48[8];
    detail::Mutex _C50;
    Emitter* _C78;
    char _C80[8];
    detail::Mutex _C88;
    char _CB0[0x10];
    detail::Mutex _CC0;
    char _CE8[0x10];
    detail::BufferAllocator mBufferAllocator;
    detail::Mutex _EE8[2];
    char _F38[0x10];
    detail::StripeSystem mStripeSystemStorage;
    detail::SuperStripeSystem mSuperStripeSystemStorage;
    char _FF8[8];
    CallbackStruct mCallbacks[0x40];
    char _2A00[0x170];
    void* mParticleCalculateCallback;
    char _2B78[0x60];
    void* mDummyRenderStateSetCallback;
    char _2BE0[0x188];
    detail::ConnectionStripeSystem* mConnectionStripeSystem;
    detail::StripeSystem* mStripeSystem;
    detail::SuperStripeSystem* mSuperStripeSystem;
    char _2D80[0x88];
    u32 _2E08[8];
    void* mDrawPathRenderStateSetCallbackArray[8];
    char _2E68[0x28];
    FindShader mFindShaderCallback;
    FindShader mFindComputeShaderCallback;
    char _2EA0[0x10];
    detail::Mutex _2EB0;
    int _2ED8[4];
    ResourceArray mResourceArray;
    char _2F08[8];
    detail::StripeAllocator mStripeAllocator;
    char _2F28[0x10];
    int mTempBufferCount;
    int mEmitterSetIndexIter;
};

}  // namespace nn::vfx2
