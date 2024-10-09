#pragma once

#include <container/seadPtrArray.h>
#include <mc/seadCoreInfo.h>
#include "mc/seadDependencyJobQueue.h"
#include <mc/seadDependencyWorker.h>
#include <prim/seadSafeString.h>
#include <thread/seadAtomic.h>

namespace sead {

class AnyFunctionBase;

class DependencyWorkerMgr {
public:
    struct InitializeArg {
        AnyFunctionBase* _00;
        AnyFunctionBase* job_run_callback;
        u8 core_count;
        SafeString worker_name;
        s32 thread_priority;
        u32 _24;
        u32 stack_size;
        u32 _2C;
    };

    DependencyWorkerMgr(Heap*, InitializeArg&);

private:
    PtrArray<WorkerHolder> mWorkers;
    DependencyJobThreadControl mMainThreadControl;
    CoreIdMask mWorkerCoreMask;
    Atomic<int> mActiveWorkers{AtomicDirectInitTag{}, 0};
    u8 mActiveWorkerMask;
    u8 mCoreFlags;
    DependencyLightJobQueue* mJobQueue;
};

} // namespace sead