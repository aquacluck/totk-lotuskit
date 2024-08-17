#pragma once

#include <prim/seadRuntimeTypeInfo.h>
#include <container/seadObjArray.h>
#include <container/seadOffsetList.h>
#include <mc/seadCoreInfo.h>
#include <mc/seadJob.h>

namespace sead {

struct JobNode {
    u16 register_id;
    u16 priority;
    u16 duplicate_count;
    u8 enable_duplicates;
    u8 queue_flags;
    Job* job;
    int _10;
};

struct JobGraphNode {
    Job* job;
    u16 queue_array_index;
    u16 priority;
    u32 duplicate_count;
    OffsetList<JobGraphNode*> dependents_list;
    s32 dependent_count;
    u8 remove_duplicates;
    CoreId core_id;
    int _34;
};

struct JobNodeRegisterLink {
    u16 parent_id;
    u16 dependent_id;
    int _04;
};

struct JobNodeUserLink {
    u8 parent_user_id;
    u8 dependent_user_id;
    bool has_register_link;
    char _03[6];
};

struct JobGraphLink {
    JobGraphNode* node;
    ListNode dependency_list_node;
};

class DependencyJobGraph {
    SEAD_RTTI_BASE(DependencyJobGraph)
public:
    struct InitializeArg {
        u16 max_job_count;
        u16 max_job_link_count;
    };

    DependencyJobGraph(Heap*, InitializeArg&);

    virtual ~DependencyJobGraph();

    struct RegisterArg {
        u8 id;
        u8 _01[7];
        Job* job;
        bool enable_duplicates;
        u16 priority;
        u16 duplicate_count;
        int _18;
        char _1C[4];
    };

    u16 convertUserToRegisterId(u8) const;
    u16 registerJob(RegisterArg&);
    void addDepedencyByUserId(u8 parent_id, u8 dependent_id);
    void addDepedencyByRegisterId(u16 parent_id, u16 dependent_id);
    void addDepedencyByDependentUserId(u16 parent_id, u8 dependent_id);

private:
    ObjArray<JobNode> mJobs;
    u16 mRegisterId;
    ObjArray<JobNodeRegisterLink> mRegisterLinks;
    ObjArray<JobNodeUserLink> mUserLinks;
    u16 mUserToRegisterIdMap[256];
};

} // namespace sead