#pragma once

#include <container/seadBuffer.h>
#include <container/seadFreeList.h>
#include <container/seadPtrArray.h>
#include <thread/seadCriticalSection.h>

namespace sead {
class Heap;
} // namespace sead

namespace agl {

class CompactibleHeap;
class TextureCompactionInfo;
class TextureCompactionJob;

class TextureCompactionMgr {
public:
    void initialize(sead::Heap* heap, int job_count);

private:
    sead::CriticalSection _00;
    sead::CriticalSection mJobCS;
    sead::Buffer<TextureCompactionJob> mCompactionJobs;
    int mActiveJobCount;
    sead::PtrArray<TextureCompactionJob> mActiveJobs;
    char _68[8];
    sead::PtrArray<TextureCompactionInfo> mCompactionInfoArray;
    sead::FreeList mFreeList;
    int mWorkMemorySize;
    u8 _9C;
    u8 _9D;
    u8 mHasWrittenToWorkBuffer; // probably wrong
    u8 _9F;
    CompactibleHeap* mCompactibleHeap;
};

} // namespace agl