#pragma once

#include "heap/seadFrameHeap.h"

namespace sead {
class SpareFrameHeap : public FrameHeap {
    SEAD_RTTI_OVERRIDE(SpareFrameHeap, FrameHeap)
public:
    ~SpareFrameHeap() override;

    // TODO
private:
    void* mSpareMemory;
    int mSpareMemorySize;
    int mSpareMemoryAlignment;
};
} // namespace sead