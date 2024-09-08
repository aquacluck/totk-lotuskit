#pragma once

#include <container/seadListImpl.h>
#include <container/seadObjList.h>
#include <container/seadSafeArray.h>
#include <nn/os/os_ReaderWriterLock.h>

namespace agl {

class CompactibleMemBlock {
public:
    // TODO

private:
    u32 mState;  // deadcode = unallocated, adooodle = in use, disabled = free for use
    int mNextBlockOffset;
    int mPrevBlockOffset;
    int mNextCompactionBlockOffset;
    int mPrevCompactionBlockOffset;
    u32 _14;
    void* mMemory;
    size_t mMemorySize;
    int mNextHeapBlockOffset;
    int mPrevHeapBlockOffset;
    u8 _30;
    u8 mCompactionJobIndex;
    u16 mBaseOffset;
    sead::ListNode mListNode;
};

class IHeap {
public:
    virtual void v1() = 0;  // might be like free all or something but it's stubbed

    virtual ~IHeap();

    virtual CompactibleMemBlock* allocBlock(CompactibleMemBlock*, size_t, size_t, int, u16, bool) = 0;
    virtual void freeBlock(CompactibleMemBlock*) = 0;
};

// TODO: what do you call this
struct MemBlockArray {
    u8 BlockMask[0x10];
    char _10[0x78];
    // blocks are organized into a 32x32 array
    sead::SafeArray<sead::SafeArray<CompactibleMemBlock*, 32>, 32> mBlockArray;
};

class Heap : public IHeap {
public:
    Heap(size_t, uintptr_t, size_t, void*, u64, bool, bool);

    void v1() override; // TOOD: double check signature

    ~Heap() override;

    CompactibleMemBlock* allocBlock(CompactibleMemBlock*, size_t, size_t, int, u16, bool) override;
    void freeBlock(CompactibleMemBlock*) override;

    virtual void finalize();
    virtual void reset();
    virtual void clearWorkspace();

    virtual void* alloc(size_t, int, int, int, bool);
    virtual void free(void*);

protected:
    nn::os::ReaderWriterLock mLock;
    u8 _38;
    u8 mAddressMaxBitCount;
    char _3A[6];
    u8 mBlockMask[0x10];
    char _50[0x60];
    // unsure why this one is 32x27 and not 32x32 like the others
    sead::SafeArray<sead::SafeArray<CompactibleMemBlock*, 32>, 27> mMemBlocks;
    MemBlockArray* mFreeBlocks;
    MemBlockArray* mUsedBlocks;
    CompactibleMemBlock* mCurrentBlock;
    CompactibleMemBlock* mStart;
    CompactibleMemBlock* mEnd;
    size_t mFreeSize;
    sead::ObjList<CompactibleMemBlock> mBlockList;
    u8 _1C10;
    void* mTextureHeapMemory;
    size_t mTextureHeapSize;
};

}  // namespace agl