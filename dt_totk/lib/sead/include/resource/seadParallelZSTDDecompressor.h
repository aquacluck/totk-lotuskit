#ifdef SEAD_ZSTD
#ifndef SEAD_PARALLELZSTDDECOMPRESSOR_H_
#define SEAD_PARALLELZSTDDECOMPRESSOR_H_

#include <mc/seadCoreInfo.h>
#include <resource/seadZSTDDecompressor.h>
#include <thread/seadEvent.h>
#include <thread/seadThread.h>

namespace sead {

class ZSTDDecompThread : Thread {
public:
    ~ZSTDDecompThread() override;

private:
    void calc_(MessageQueue::Element msg) override;

    ZSTDDecompressor::DecompContext mContext;
    Event mDecompressionFinishEvent;
    Event mReadLoopEvent;
    void* mInputStream;
    u32 mDecompressedSize;
    u32 mStreamSize;
    u32 mLastReadSize;
    void* _188;
};

class ParallelZSTDDecompressor : ZSTDDecompressor {
public:
    ParallelZSTDDecompressor(s32 read_size, s32 thread_prio, Heap* heap, ZSTD_DCtx* dctx, CoreIdMask& mask);
    
    ~ParallelZSTDDecompressor() override;

    virtual u8* tryDecompFromDevice(const ResourceMgr::LoadArg& loadArg, Resource* resource,
                                    u32* outSize, u32* outAllocSize, bool* outAllocated) override;
    
private:
    u32 mReadSize;
    ZSTD_DCtx* mDCtx;
    ZSTDDecompThread mDecompThread;
};

}  // namespace sead

#endif  // SEAD_PARALLELZSTDDECOMPRESSOR_H_
#endif  // SEAD_ZSTD