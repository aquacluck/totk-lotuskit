#pragma once

#include <nn/types.h>

namespace nn::vfx2 {

class Emitter;

namespace detail {
class Heap;
class Stripe;
class StripeInstance;

class StripeSystem {
public:
    bool InitializeSystem(Heap*, void*, int, u64, u64, u64, Emitter*);

    void ParticleCalculateCallback();

    static int GetExtendedEndTimeForOneTimeEmitter(Emitter*);

private:
    Stripe* mStripes;
    StripeInstance* mStripeInstance;
    char _18[0x20];
    int _30[2];
    size_t mBufferSize;
    char _40[8];
    int mStripeCount;
    char _4C[0xc];
};

} // namespace detail

} // namespace nn::vfx2