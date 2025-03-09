#pragma once

#include <nn/gfx/gfx_Device.h>
#include <nn/gfx/gfx_State.h>
#include <nn/gfx/gfx_Types.h>
#include "nn/gfx/gfx_Common.h"

namespace nn::vfx2::detail {

struct ResRenderState;

// Nintendo forgot to capitalize the c in context but that's annoying so whatever
class RenderContext {
public:
    void Initialize(gfx::TDevice<gfx::DefaultApi>*, ResRenderState*);

private:
    char _00[0x20];
    ResRenderState* mRenderState;
    gfx::BlendState mBlendState;
    gfx::DepthStencilState mDepthStencilState;
    gfx::RasterizerState mRasterizerState;
};

} // namespace nn::vfx2::detail