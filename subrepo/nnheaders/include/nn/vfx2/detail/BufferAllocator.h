#pragma once

#include <nn/gfx/gfx_Buffer.h>
#include <nn/gfx/gfx_Common.h>
#include <nn/gfx/gfx_GpuAddress.h>
#include <nn/gfx/gfx_MemoryPool.h>
#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>

namespace nn::vfx2::detail {

class BufferAllocator {
public:
    struct InitializeArg;

    bool Initialize(InitializeArg&, void*, void*);
    void Finalize(gfx::detail::DeviceImpl<gfx::ApiVariationNvn8>*);

    void* Alloc(size_t, u64);
    void Free(void*, bool);

    void FlushFreeList();

private:
    void _Free(void*);

    char _00[8];
    gfx::GpuAddress mGpuAddress;
    gfx::Buffer mBuffer;
    gfx::MemoryPool mMemoryPool;
    char _180[0x20];
    void* mMappedMemory;
    char _1A8[0x20];
    size_t mBufferSize;
    size_t mAllocatedSize;
    u64 mBufferAlignment;
    int mAllocCount;
    int mMaxAllocCount;
    int _1E8;
    u8 _1EC;
};
static_assert(sizeof(BufferAllocator) == 0x1f0);

} // namespace nn::vfx2::detail