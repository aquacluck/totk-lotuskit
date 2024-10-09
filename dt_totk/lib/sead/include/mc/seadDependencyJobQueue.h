#pragma once

#include <container/seadBuffer.h>
#include <container/seadPtrArray.h>
#include <mc/seadCoreInfo.h>
#include <mc/seadDependencyJobGraph.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <thread/seadCriticalSection.h>

namespace sead {

class DependencyJobThreadControl;

struct PriorityQueue {
    JobGraphNode** nodes;
    int node_capacity;
    int node_count;
};

class DependencyLightJobQueue {
    SEAD_RTTI_BASE(DependencyLightJobQueue);
public:
    virtual ~DependencyLightJobQueue();

    u32 registerGraph(DependencyJobGraph*, bool);
    void processQueue(DependencyJobThreadControl*);
    void updatePriorityQueue();

private:
    Buffer<JobGraphNode> mJobs;
    Buffer<JobGraphLink> mLinks;
    PtrArray<JobGraphNode> mUsedJobs;
    PtrArray<DependencyJobThreadControl> mThreadControlArray;
    PriorityQueue mPriorityQueue;
    CriticalSection mQueueCS;
    u32 mGraphLinkCount;
    JobGraphNode mDestinationNode;
    CoreId mPrimaryThreadId;
    u32 mQueueFlags;
    u32 mPriorityQueueJobCount;
};

} // namespace sead