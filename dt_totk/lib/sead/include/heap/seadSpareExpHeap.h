#pragma once

#include "heap/seadExpHeap.h"

namespace sead {
class SpareExpHeap : public ExpHeap {
    SEAD_RTTI_OVERRIDE(SpareExpHeap, ExpHeap)
public:
    ~SpareExpHeap() override;

    // TODO

private:
    void* mSpareMemory;
    int mSpareMemorySize;
    int mSpareMemoryAlignment;
};
}  // namespace sead