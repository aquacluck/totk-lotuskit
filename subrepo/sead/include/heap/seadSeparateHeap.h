#pragma once

#include "container/seadObjList.h"
#include "heap/seadHeap.h"
#include "heap/seadMemBlock.h"

namespace sead
{
class SeparateHeap : public Heap
{
    SEAD_RTTI_OVERRIDE(SeparateHeap, Heap)
public:
    ~SeparateHeap() override;

    // TODO
private:
    ObjList<MemBlock> mMemBlockList;
};
}  // namespace sead