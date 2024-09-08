#pragma once

#include <basis/seadTypes.h>
#include <container/seadListImpl.h>
#include <heap/seadSeparateHeap.h>
#include <prim/seadSafeString.h>
#include <nvn/nvn.h>

namespace agl {

class GPUMemBlockBase;

namespace detail {

class GPUMemBlockMgrHeapEx;
class MemoryPoolType;

class MemoryPoolHeap {
public:
    // TODO: verify these methods are still accurate
    MemoryPoolHeap(void*, u64, u64, const MemoryPoolType&, void*, u64, GPUMemBlockMgrHeapEx*);
    virtual ~MemoryPoolHeap();

    static MemoryPoolHeap* create(u64, s32, u64, s32, u64, u64, const MemoryPoolType&,
                                  GPUMemBlockMgrHeapEx*);
    static void destroy(MemoryPoolHeap*);

    void pushBack(GPUMemBlockBase*);
    u64* allocFromMemoryPool(u64, s32);
    void freeToHeap(GPUMemBlockBase*);
    bool isAllocatable(const MemoryPoolType&, u64, s32) const;

private:
    u32 _08;
    u32 _0C;
    GPUMemBlockMgrHeapEx* mMgrHeap;
    void* mMemory;
    sead::ListNode _20;
    u32 _30;
    u32 _34;
    size_t mSize;
    NVNmemoryPool mNvnMemoryPool;
    void* mMappedMemory;
    u32 mFlags;
    u32 mIndex;
    char _150[8];
    sead::ListNode _158;
    sead::ListNode _168;
    sead::SafeString mName;
    sead::SeparateHeap* mGpuHeap;
    u32 mMaxAllocatbleSize;
    u32 mBlockSize;
    int _190;
    int _194;
    // heap follows immediately
};

}  // namespace detail
}  // namespace agl
