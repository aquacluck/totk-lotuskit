#ifndef SEAD_PARALLELSZSDECOMPRESSOR_H_
#define SEAD_PARALLELSZSDECOMPRESSOR_H_

#include <mc/seadCoreInfo.h>
#include <resource/seadSZSDecompressor.h>
#include <thread/seadEvent.h>
#include <thread/seadThread.h>

namespace sead {

class SZSDecompThread : public Thread {
public:
    ~SZSDecompThread() override;

private:
    void calc_(MessageQueue::Element msg) override;

    SZSDecompressor::DecompContext mContext;
    Event mDecompressionFinishEvent;
    Event mReadLoopEvent;
    void* mInputStream;
    u32 mDecompressedSize;
    u32 mStreamOffset;
    u32 mLastReadSize;
};

class ParallelSZSDecompressor : SZSDecompressor {
public:
    ParallelSZSDecompressor(s32 read_size, s32 thread_prio, Heap* heap, void* memory, CoreIdMask& mask);
    
    ~ParallelSZSDecompressor() override;

    virtual u8* tryDecompFromDevice(const ResourceMgr::LoadArg& loadArg, Resource* resource,
                                    u32* outSize, u32* outAllocSize, bool* outAllocated) override;

private:
    SZSDecompThread mDecompThread;
};

} // namespace sead

#endif // SEAD_PARALLELSZSDECOMPRESSOR_H_