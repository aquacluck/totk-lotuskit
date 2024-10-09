#pragma once

#include <framework/seadGameFramework.h>
#include <gfx/seadColor.h>
#include <gfx/seadFrameBuffer.h>
#include <math/seadVector.h>
#include <nvn/nvn.h>
#include <thread/seadThread.h>
#include <thread/seadDelegateThread.h>
#include "nn/os.h"

namespace nn
{
namespace mem
{
class StandardAllocator;
}
namespace vi
{
class Display;
class Layer;
}
}  // namespace nn

namespace sead
{
class DisplayBufferNvn;

class GameFrameworkNx : public GameFramework
{
    SEAD_RTTI_OVERRIDE(GameFrameworkNx, GameFramework)
public:
    struct CreateArg
    {
        s32 mVBlankWaitInterval;
        Color4f mClearColor;
        Vector2i mFrameBufferDimensions;
        bool mCreateFrameBufferNVN;
        bool mIsTripleBuffer;
        char _1E[6];
        u32 mMemoryPoolSize;
        u32 mCommandBufferMemorySize;
        u32 _2C;
        u32 mNvGraphicsHeapSize;
        u32 mNvGraphicsDevtoolsAllocatorSize;
        u32 mQueueComputeMemorySize;
        u32 mQueueCommandMemorySize;
        u32 mQueueControlMemorySize;
        u32 mDebugFlags;
        u32 _48;
        s32 mPresentationThreadPriority;
    };
    static_assert(sizeof(CreateArg) == 0x50);

    explicit GameFrameworkNx(const CreateArg&);
    ~GameFrameworkNx() override;

    void initializeArg(const Framework::InitializeArg&);
    void initializeGraphicsSystem(Heap*, Heap*, const Vector2f&);
    void outOfMemoryCallback_(NVNcommandBuffer*, NVNcommandBufferMemoryEvent, size_t, void*);
    void presentAsync_(Thread*, long);
    void getAcquiredDisplayBufferTexture() const;  // unknown return type
    void setVBlankWaitInterval(u32);
    void requestChangeUseGPU(bool);
    void getGraphicsDevToolsAllocatorTotalFreeSize() const;  // unknown return type
    void initRun_(Heap*) override;
    void runImpl_() override;
    FrameBuffer* getMethodFrameBuffer(int) const override;
    LogicalFrameBuffer* getMethodLogicalFrameBuffer(int) const override;
    MethodTreeMgr* createMethodTreeMgr_(Heap*) override;
    
    virtual void setCaption(const SafeString&);
    virtual void mainLoop_();
    virtual void procFrame_();
    virtual void procDraw_();
    virtual void procCalc_();
    virtual void present_();
    virtual void swapBuffer_();
    virtual void clearFrameBuffers_(int);
    virtual void waitForGpuDone_();
    virtual void setGpuTimeStamp_();  // unknown return type

    void waitVsyncEvent_();

private:
    CreateArg mCreateArg;
    u64 mDeltaTick;
    u64 mSystemTick;
    FrameBuffer* mMethodFrameBuffer;
    LogicalFrameBuffer mMethodLogicalFrameBuffer;
    char _110[8];
    u64 mGpuTimeStamp;
    u64 mPostAcquireTexturesTimeStamp;
    u64 mPostTextureSyncTimeStamp;
    char _130[0x10];
    DisplayBufferNvn* mDisplayBuffer;
    char _148[24];
    NVNmemoryPool* mControlMemory;
    NVNcommandBuffer* mPrimaryCommandBuffer;
    NVNcommandBuffer* mCommandBuffer;
    NVNbuffer* mBufferReport;
    NVNcounterData* mCounterDataArray;
    NVNmemoryPool* mShaderScratchMemory;
    u32 mShaderScratchMemorySize;
    ::nn::mem::StandardAllocator* mGraphicsDevToolsAllocator;
    size_t mMaxCommandMemoryUsed;
    size_t mMaxControlMemoryUsed;
    NVNqueue* mQueue;
    void* mDisplayFrameBuffer;
    ::nn::vi::Display* mDisplay;
    ::nn::vi::Layer* mDisplayLayer;
    DelegateThread* mPresentationThread;
    NVNsync* mQueueSubmitFenceSync;
    char _1E0[8];
    void* mLastCommandList;
    ::nn::os::SystemEvent mVSyncEvent;
    char _220[4];
    char _224[0xc];
};
static_assert(sizeof(GameFrameworkNx) == 0x230);

}  // namespace sead
