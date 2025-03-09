#pragma once

#include <gfx/seadGraphics.h>
#include <thread/seadAtomic.h>
#include <thread/seadCriticalSection.h>
#include "framework/nx/seadGameFrameworkNx.h"
#include "gfx/seadDrawLockContext.h"
#include "nn/gfx/gfx_Types.h"
#include "nvn/nvn.h"

namespace sead
{
class GraphicsNvn : public Graphics
{
public:
    class CreateArg;

    explicit GraphicsNvn(const CreateArg& arg);

    DrawLockContext* getDrawLockContext() const override;

    void initializeDrawLockContext(Heap*);
    void initializeImpl(Heap*);

    int getNewTextureId();
    int getNewSamplerId();
    NVNdevice* getNvnDevice() const { return mNvnDevice; }
    NVNtexturePool* getTexturePool() { return &mNvnTexturePool; }
    int getTextureSamplerID() const { return mTextureSamplerID; }
    CriticalSection* getTexturePoolCS() { return &mTexturePoolCS; }

private:
    using DebugCallback = IDelegate1<GraphicsNvn*>;

    NVNdevice* mNvnDevice;
    NVNqueue* mNvnQueue;
    NVNbuffer* mNvnBufferReport;
    ::nn::gfx::Device* mDevice;
    DisplayBufferNvn* mDisplayBufferNvn;
    NVNtexturePool mNvnTexturePool;
    NVNsamplerPool* mNvnSamplerPool;
    void* _80;
    void* _88;
    void* _90;
    NVNsampler mNvnSampler;
    void* _C8;
    void* _D0;
    int mTextureSamplerID;
    int _DC;
    NVNmemoryPool* mNvnMemoryPool;
    Atomic<int> mTextureSamplerIdIter;
    Atomic<int> mTextureIdIter;
    int mTextureCount;
    int mTextureSamplerCount;
    CriticalSection mCriticalSection1;
    CriticalSection mTexturePoolCS;
    CriticalSection mSamplerPoolCS;
    void* _1D8;
    void* _1E0;
    void* _1E8;
    void* _1F0;
    DebugCallback* mNvnDebugCallback;
    bool _200;
    bool _201;
    bool _202;
};
static_assert(sizeof(GraphicsNvn) == 0x188);

}  // namespace sead