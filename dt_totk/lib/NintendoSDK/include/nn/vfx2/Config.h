/**
 * @file Config.h
 * @brief VFX configuration.
 */

#pragma once

#include <nn/types.h>

namespace nn {
namespace vfx2 {
class Config {
public:
    virtual ~Config();

private:
    char _08[0x28];
    size_t mDoubleBufferSize;
    size_t _38;
    int mMaxEmitterNum;
    int mMaxEmitterSetNum;
    int mMaxStripeNum;
    int mMaxSuperStripeNum;
    int mMaxResourceNum;
    int _54[5];
    char _68;
    void* _70; // sead::Heap
    void* mDynamicHeap; // sead::Heap
    void* mEffectModelHeap; // sead::Heap
    char _88[0x10];
    int _98; // also max resource count?
    int mDynamicHeapSize;
    int mMemoryPoolSize;
    int mEffectModelHeapSize;
    bool mEnableParticleLinearDepth;
};

}  // namespace vfx2
}  // namespace nn
