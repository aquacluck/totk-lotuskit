#pragma once

#include "heap/seadHeap.h"

namespace sead {
class VirtualAddressHeap : public Heap {
    SEAD_RTTI_OVERRIDE(VirtualAddressHeap, Heap)
public:
    ~VirtualAddressHeap() override;
// TODO

};

} // namespace sead