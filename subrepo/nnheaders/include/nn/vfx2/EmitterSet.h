#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/util/MathTypes.h>
#include <nn/types.h>

namespace nn::vfx2 {

class Emitter;
class EmitterResource;
class EmitterSetResource;
class Heap;
class Resource;
class System;

class EmitterSet {
public:
    bool Initialize(Resource* resource, int eset_idx, int idx, u8 a4, void* a5);

    bool Finalize();
    bool FinalizeEmitterList(Emitter*);

    void ResetCommon();

    Emitter* CreateEmitter(EmitterResource* resource, int a2, Emitter* parent, int index);
    Emitter* GetEmitter(int) const;

    void Calculate(float, u8, bool, void*, u64, u64, void*);
    bool CalculateEmitter(float, Emitter*, u8, bool, void*, u64, u64);
    void ForceCalculate(float);

    void SetEmissionRate(float);
    void SetEmissionInterval(float);
    void SetLifeScale(float);

private:
    EmitterSet* mNext;
    Emitter* _08;
    EmitterSetResource* mResource;
    u64 _18[3];
    u32 _30[2];
    char _38;
    u32 _3C;
    char _40[8];
    float _48;
    float mAlpha;
    char _50[8];
    float _58;
    float mLifeScale;
    int _60[2];
    System* mVfxSystem;
    char _70[0x10];
    Emitter* _80;
    gfx::Device* mDevice;
    char _90[0x18];
    void* mSystemCallbackStruct; // struct with a bunch of vfx system callbacks
    char _B0;
    Heap* mHeap;
    char _C0[0xd0];
    util::Float3 mVelocity;
    char _19C[0x14];
    util::Float3 mRotation;
    char _1BC[0x14];
    util::Float3 mScale;
    char _1DC[0x54];
    int _230;
    int mIndex;
    char _238[0x14];
    float mEmissionRate;
    float mEmissionInterval;
    float mDirectionalVel0;
    float _258;
    float mDirectionVel1;
    char _260[4];
    u32 mRandomSeed;
    u32 _268[2];
    char _270[0x10];

};

} // namespace nn::vfx2