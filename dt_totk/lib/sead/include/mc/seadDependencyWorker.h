#pragma once

#include <container/seadRingBuffer.h>
#include <mc/seadCoreInfo.h>
#include <mc/seadDependencyJobGraph.h>
#include <mc/seadDependencyJobQueue.h>
#include <prim/seadSafeString.h>
#include <thread/seadCriticalSection.h>
#include <thread/seadEvent.h>
#include <thread/seadThread.h>

namespace sead {

class AnyFunctionBase;

class DependencyJobThreadControl {
public:
    DependencyJobThreadControl();
    virtual ~DependencyJobThreadControl();

private:
    JobGraphNode* mNextNode;
    u32 mRunFlags;
    FixedRingBuffer<JobGraphNode*, 8> mSuggestedJobs;
    CriticalSection mSuggestQueueCS;
    Event mOutOfJobsEvent;
    void* mRunJobCallback;
    void* mRunJobCallbackArg;
    int _D0;
    u8 _D4;
    CoreId mCoreId;
};

class DependencyWorkerBase : Thread {
public:
    ~DependencyWorkerBase() override;

protected:
    void run_() override;

    DependencyJobThreadControl mThreadControl;
    AnyFunctionBase* mThreadRunCallback;
    Event mWorkerFinishEvent;
    CoreId mCoreId;
};

class DependencyWorker : DependencyWorkerBase {
public:
    ~DependencyWorker() override;

private:
    void calc_(MessageQueue::Element msg) override;

    DependencyLightJobQueue mJobQueue;
};

struct WorkerHolder {
    HeapSafeString* mThreadName;
    DependencyWorker* mWorker;
};

} // namespace sead