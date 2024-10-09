#pragma once

#include "heap/seadHeap.h"
#include <nn/types.h>

namespace sead {
class UnitHeap : public Heap {
    SEAD_RTTI_OVERRIDE(UnitHeap, Heap)
public:
    ~UnitHeap() override;

    static UnitHeap* tryCreate(size_t, SafeString&, u32, u32, Heap*, bool);

    // TODO

private:
    u32 mUnitSize;
    void* mHead;
    size_t mSize;
    size_t mTotalUnitSize;
    void* _C8;
    u64 _D0;
};

} // namespace sead