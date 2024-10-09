#pragma once

#include <nn/types.h>

namespace nn::vfx2::detail {

struct ResTextureSamplerInfo {
    size_t mGUID;
    u8 mIndices[3];
    char _0B[13];
};

} // namespace nn::vfx2::detail