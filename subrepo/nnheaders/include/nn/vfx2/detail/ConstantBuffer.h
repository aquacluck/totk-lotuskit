#pragma once

#include <nn/gfx/gfx_Buffer.h>
#include <nn/gfx/gfx_GpuAddress.h>
#include <nn/gfx/gfx_MemoryPool.h>
#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>

namespace nn::vfx2::detail {

class ConstantBuffer {
public:
    ConstantBuffer();

private:
    gfx::Buffer mBuffer;
    gfx::MemoryPool mMemoryPool;
    u64 mAlignment;
    size_t mSize;
    char _178[0x10];
    gfx::GpuAddress mGpuAddress;
    size_t mOffset;
    void* mStart;
};

} // namespace nn::vfx2::detail