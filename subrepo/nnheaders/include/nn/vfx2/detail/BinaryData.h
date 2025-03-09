#pragma once

#include <nn/types.h>

namespace nn::vfx2::detail {

struct BinaryData {
    char mMagic[4];
    u32 mSize;
    int mNextSectionOffset; // next section in the highest array level
    int mNextSubsectionOffset; // next section in the second highest array level
    int mNextSubSectionOffset; // next section in the third highest array level
    u32 mDataOffset;
};

} // namespace nn::vfx2::detail