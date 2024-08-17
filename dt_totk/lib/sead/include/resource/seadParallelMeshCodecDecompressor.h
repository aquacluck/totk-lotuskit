#include "thread/seadMessageQueue.h"
#ifdef SEAD_ZSTD
#ifndef SEAD_PARALLELMESHCODECDECOMPRESSOR_H_
#define SEAD_PARALLELMESHCODECDECOMPRESSOR_H_

#include <mc/seadCoreInfo.h>
#include <nn/os.h>
#include <resource/seadMeshCodecDecompressor.h>
#include <thread/seadThread.h>

namespace sead {

class MeshCodecDecompThread : Thread {
public:
    ~MeshCodecDecompThread() override;

private:
    void calc_(MessageQueue::Element msg) override;

    MeshCodecDecompressor::DecompContext mContext;
    nn::os::LightEventType mFinishEvent;
};

class ParallelMeshCodecDecompressor : MeshCodecDecompressor {
public:
    ParallelMeshCodecDecompressor(s32 thread_prio, Heap* heap, void* memory, size_t mem_size, u64,
                                  CoreIdMask& mask);

    ~ParallelMeshCodecDecompressor() override;

    virtual u8* tryDecompFromDevice(const ResourceMgr::LoadArg& loadArg, Resource* resource,
                                    u32* outSize, u32* outAllocSize, bool* outAllocated) override;

private:
    MeshCodecDecompThread mDecompThread;
};

}  // namespace sead

#endif  // SEAD_PARALLELMESHCODECDECOMPRESSOR_H_
#endif  // SEAD_ZSTD