#pragma once

#include <container/seadFreeList.h>
#include <container/seadPtrArray.h>
#include <container/seadTreeMap.h>
#include <heap/seadDisposer.h>
#include <prim/seadDelegate.h>
#include <thread/seadCriticalSection.h>
#include <thread/seadDelegateThread.h>
#include <thread/seadMessageQueue.h>
#include "common/aglTexture.h"

namespace sead {
class UnitHeap;
} // namespace sead

namespace agl {

struct BlockHandle;
class CompactibleHeap;
class TextureCompactionMgr;

class TextureDataMgr {
public:
    TextureDataMgr();

    struct InitializeArg {
        sead::Heap* heap;
        sead::Heap* debug_heap;
        u32 texture_handle_num;
        int _14;
        size_t texture_heap_size;
        size_t compaction_work_heap_size;
        u32 compaction_job_num;
        int _2C[5];
        u64 cache_rejection_threshold;
        u8 _48;
    };

    void initialize(InitializeArg&);

    // TODO: everything else

private:
    using ThreadDelegate = sead::Delegate2<TextureDataMgr, sead::DelegateThread*, sead::MessageQueue::Element> ;

    // probably should be sead::ObjMap<TextureHandle> but sead::TreeMap needs fixing first
    sead::TreeMapNode<TextureHandle>* mRootNode;
    sead::FreeList mFreeList;
    int mHandleCount;
    int mHandleCapacity;
    sead::OffsetList<Texture> mTextures;
    sead::UnitHeap* mTextureHeap;
    CompactibleHeap* mCompactibleHeap;
    TextureCompactionMgr* mCompactionMgr;
    TextureMemoryPool mTextureMemoryPool;
    ThreadDelegate mCompactionCallback;
    sead::DelegateThread mCompactionDelegate;
    int mCompactionStatus;
    ThreadDelegate mReplaceCallback;
    sead::DelegateThread mReplaceDelegate;
    int mReplaceStatus;
    void* mTextureMemory;
    u64 mTextureCacheRejectionThreshold;
    sead::OffsetList<TextureInfo> mReplacementRequests;
    sead::OffsetList<TextureInfo> mPendingReplacements;
    sead::OffsetList<TextureInfo> mProcessedReplacements;
    int mCount;
    int mCapacity;
    sead::OffsetList<BlockHandle> mMemoryBlocks;
    sead::Heap* mInitializeHeap;
    u32 _3C0;
    char _3C4[0xc];
    sead::CriticalSection mTextureListCS;
    sead::CriticalSection mBlockListCS;
    sead::CriticalSection _410;

    SEAD_SINGLETON_DISPOSER(TextureDataMgr)
};

} // namespace agl