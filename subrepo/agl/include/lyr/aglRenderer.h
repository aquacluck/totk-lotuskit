#pragma once

#include <container/seadBuffer.h>
#include <container/seadPtrArray.h>
#include <heap/seadDisposer.h>
#include <mc/seadJobQueue.h>
#include "thread/seadCriticalSection.h"
#include "lyr/aglRenderDL.h"

namespace sead {
class DrawMethod;
class FrameBuffer;
class Heap;
} // namespace sead

namespace agl::lyr {

class Layer;
class LayerJob;

class RenderStep {
public:
    RenderStep();
    // TODO

private:
    char _00[8]; // might be the vtable, I forget
    sead::FixedPtrArray<sead::DrawMethod, 256> mDrawMethods;
    u8 _818;
};

class Renderer {
public:
    struct InitializeArg {
        virtual ~InitializeArg();

        char _08[0xb0];
        sead::FrameBuffer* _B8[2];
        char _C8[0x30];
    };

    void initialize(InitializeArg&, sead::Heap*, sead::Heap*);

private:
    u32 _08[4];
    void** _18;
    sead::PtrArray<Layer> mLayers;
    sead::Buffer<LayerJob> mLayerJobArrays[3];
    RenderDLBuffer mDisplayListBuffer;
    u32 mTargetFormat; // probably just TextureFormat but needs verification
    char _1E4[0x34];
    sead::FixedSizeJQ mJobQueueArray[4];
    sead::PtrArrayImpl _458[2];
    u32 _478[4];
    sead::CriticalSection mCS;
    char _4A8[0x28];

    SEAD_SINGLETON_DISPOSER(Renderer)
};

} // namespace agl::lyr