#pragma once

#include <nn/vfx2/detail/ResEmitterSet.h>
#include <nn/types.h>

namespace nn::vfx2 {

namespace detail {
struct ResEmitterSet;
} // namespace detail

class EmitterResource;

class EmitterSetResource {
public:

private:
    detail::ResEmitterSet* mResEmitterSet;
    EmitterResource* mEmitters;
    EmitterResource* _10;
    char _18[8];
    u16 mEmitterResourceCount;
    u8 mEmitterSectionCount;
    char _23[4];
};

} // namespace nn::vfx2