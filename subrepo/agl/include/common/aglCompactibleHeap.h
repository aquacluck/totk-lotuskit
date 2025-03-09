#pragma once

#include <prim/seadSafeString.h>
#include "common/aglHeap.h"

namespace agl {

class TextureCompactionMgr;

// TODO: think of a better name for this
struct CompactionTracker {
    TextureCompactionMgr* mgr;
    u8 stage;
    u8 direction;
    u8 unk_count;
    u8 flags;
    int _0C;
    u64 block_offsets[256];
    int block_count;
};

class CompactibleHeap : public Heap {
public:
    ~CompactibleHeap() override;

    void finalize() override;
    void clearWorkspace() override;

    void free(void*) override;

    // TODO: other methods

private:
    CompactionTracker* mCompactionTracker;
    sead::SafeString mName;
    CompactionTracker mTrackerStorage;  // may not be considered part of the struct idk
};

}  // namespace agl