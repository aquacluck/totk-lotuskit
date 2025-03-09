#pragma once

#include <nvn/nvn.h>
#include <container/seadListImpl.h>
#include <container/seadPtrArray.h>
#include <heap/seadDisposer.h>
#include <hostio/seadHostIONode.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadBitFlag.h>
#include <prim/seadTypedBitFlag.h>
#include <thread/seadCriticalSection.h>
#include "common/aglGPUCommon.hpp"
#include "common/aglGPUMemBlock.h"

namespace agl::detail {

using MemoryPoolDriverBitFlag = sead::BitFlag32;

constexpr s32 VALID_POOL_TYPE_VALUE = -1;
constexpr s32 cGPUAccessMask = 0xF0000000;
constexpr u64 cGPUPhysicalMemorySizeAlignment = 0x1000;

class MemoryPoolType : MemoryPoolDriverBitFlag {
public:
    MemoryPoolType() : MemoryPoolDriverBitFlag() {}
    MemoryPoolType(s32 p_value) : MemoryPoolDriverBitFlag(p_value) {}

    MemoryPoolType convert(MemoryAttribute attribute);

    bool IsValid() const { return (*this & cValidPoolType) == cValidPoolType; }

    void MarkValid() { *this = *this | cValidPoolType; }

private:
    static const MemoryPoolType cInvalidPoolType;
    static const MemoryPoolType cValidPoolType;
};

class MemoryPool {
public:
    MemoryPool();

    void initialize(void* storage_1, u64 storage_2, const MemoryPoolType& flags);
    void initialize(void* map_virtual_1, u64 storage, const MemoryPoolType& flags,
                    const MemoryPool& map_virtual_2, s32 map_virtual_3);

    void finalize();

private:
    NVNmemoryPool mDriverPool;
    MemoryPoolType mMemoryType;
    uint32_t idk;
};
static_assert(sizeof(MemoryPool) == 0x108);

class GPUMemBlockMgrHeapEx : public sead::IDisposer {
public:
    GPUMemBlockMgrHeapEx(sead::Heap* p_heap);
    ~GPUMemBlockMgrHeapEx() override;

    void finalize();

private:
    u8 _20[4];
    u32 _24;
    sead::ListImpl _28;
    sead::ListImpl mMemoryPoolHeapList;
    sead::CriticalSection mCS;
    char _78[0x10];
    u32 _88;
    u32 _8C;
};
static_assert(sizeof(GPUMemBlockMgrHeapEx) == 0x90);

enum class GPUMemBlockMgrFlags : u8 {
    MemoryPoolRelated = 1 << 0,
    EnablePoolSharing = 1 << 1,
    Debug = 1 << 2
};

class GPUMemBlockMgr {
public:
    GPUMemBlockMgr();
    virtual ~GPUMemBlockMgr();

    GPUMemBlockMgrHeapEx* createGPUMemBlockMgrHeapEx(sead::Heap*);
    bool tryAllocMemory(GPUMemBlockBase* block, sead::Heap* heap, size_t size, int alignment,
                        MemoryAttribute mem_attr, size_t min_size, const char* name, bool);

    void initialize(sead::Heap* heap1, sead::Heap* heap2);
    void enableSharedMemoryPool(bool enabled);
    static u64 calcGPUMemorySize(u64 userSize);
    static s32 calcGPUMemoryAlignment(s32 userAlignment);

#ifdef SEAD_DEBUG
    void listenPropertyEvent(const sead::hostio::PropertyEvent* event) override;
    void genMessage(sead::hostio::Context* context) override;
#endif

private:
    GPUMemBlockMgrHeapEx* findGPUMemBlockMgrHeapEx_(sead::Heap* p_heap, int* p_outIndex);

    sead::CriticalSection mCS;
    sead::PtrArray<GPUMemBlockMgrHeapEx> mMngrHeaps;
    size_t mMinBlockSize;
    sead::TypedBitFlag<GPUMemBlockMgrFlags> mFlags;
    u32 _44;
    u32 mMemoryPoolIndexIter;
    u32 _4C;
    char _50[8];
    u32 _58;  // 0x20
    u32 _5C;
    char _60[0x20];

    SEAD_SINGLETON_DISPOSER(GPUMemBlockMgr)
};
static_assert(sizeof(GPUMemBlockMgr) == 0xa0);

}  // namespace agl::detail
