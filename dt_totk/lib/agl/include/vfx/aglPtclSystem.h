#pragma once

#include <nn/vfx2/Config.h>
#include <nn/vfx2/System.h>
#include <thread/seadCriticalSection.h>

namespace sead {
class Heap;
} // namespace sead

namespace agl::vfx {

class PtclSystem : public nn::vfx2::System {
public:
    // TODO

private:
    char _2F40[8];
    u64 mSystemTick;
    nn::vfx2::Config mConfig;
    char _3000[0x28];
    sead::Heap* mHeap;
    char _3030[0x20];
    sead::Heap** mHeapArray;
    char _3058[0x10];
    void* mParticleMgr; // gsys::ParticleMgr? but gsys is dependent on agl and not the other way around
    char _3070[0xf50];
    sead::CriticalSection mEmitterCS;
    void* mAllocatorDescriptorSlotForTextureCallback;
    void* mReleaseDescriptorSlotForTextureCallback;
    void* mAllocatorDescriptorSlotForSamplerCallback;
    void* mReleaseDescriptorSlotForSamplerCallback;
    u32 _4000;
};

} // namespace agl