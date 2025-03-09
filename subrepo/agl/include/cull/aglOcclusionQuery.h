#pragma once

#include <math/seadVector.h>
#include <nvn/nvn.h>
#include "common/aglGPUMemBlock.h"

namespace agl::cull {

class OcclusionQuery {
public:
    // idk

private:
    char _00[8];
    u16 _08[4];
    NVNbuffer mNvnBuffer;
    GPUMemBlock<u8> mGpuMemBlock;
    sead::Vector4f _80;
};

} // namespace agl::cull