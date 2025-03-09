#pragma once

#include <container/seadPtrArray.h>
#include <gfx/seadViewport.h>
#include "common/aglRenderBuffer.h"
#include "common/aglRenderTarget.h"
#include "common/aglTextureData.h"
#include "lyr/aglRenderDL.h"

namespace agl::lyr {

class Layer;
class RenderDLBuffer;

class RenderDisplay {
public:
    // TODO

private:
    u8 _00[8];
    char _08[0x10];
    RenderBuffer mRenderBuffer;
    char _80[0x18];
    TextureData mColorData;
    TextureData mDepthData;
    RenderTargetColor mRenderTargetColor;
    char _2C8[0x20];
    RenderTargetDepth mRenderTargetDepth;
    char _3B8[0x20];
    RenderTargetColor _3D8;
    char _4A8[0x20];
    RenderBuffer _4C8;
    char _530[0x18];
    TextureData mRenderDisplayResolve;
    RenderTargetColor _5F8;
    char _6C8[0x40];
    RenderDLBuffer* mRenderDLBuffer;
    char _710[8];
    sead::Viewport mViewport;
    char _740[8];
    sead::PtrArray<Layer> mLayers;
    char _758[0x48];
    sead::PtrArray<RenderDL> mFinishedDisplayLists;
    char _7B0[0x30];
};

} // namespace agl::lyr