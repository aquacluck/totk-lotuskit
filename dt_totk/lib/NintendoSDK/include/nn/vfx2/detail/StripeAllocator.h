#pragma once

namespace nn::vfx2::detail {

class Heap;

class StripeAllocator {
public:
    virtual ~StripeAllocator();

private:
    Heap* mHeap;
    int mStripeType;
};

} // namespace nn::vfx2::detail