#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>
#include <nn/vfx2/detail/ConstantBuffer.h>
#include "nn/gfx/gfx_Common.h"
#include "nn/gfx/gfx_Device.h"

namespace nn::vfx2 {

class TemporaryBuffer {
public:
    bool Initialize(gfx::TDevice<gfx::DefaultApi>*, u64, size_t, u8, u64);

private:
    detail::ConstantBuffer mBuffers[3];
    char _4F8[0x28];
    size_t mBufferSize;
    char _528[5];
    bool mIsInitialized;
    char _52E[0x2fa];

};

} // namespace nn::vfx2