#pragma once

#include "heap/seadHeap.h"
#include "heap/seadMemBlock.h"
#include "container/seadObjList.h"

namespace sead {
class SeparateHeap : public Heap {
    SEAD_RTTI_OVERRIDE(SeparateHeap, Heap)
public:
    ~SeparateHeap() override;

    // TODO
private:
    ObjList<MemBlock> mMemBlockList;
};
} // namespace sead