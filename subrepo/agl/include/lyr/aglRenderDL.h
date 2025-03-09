#pragma once

#include "common/aglDisplayList.h"
#include "common/aglGPUMemBlock.h"
#include "container/seadBuffer.h"
#include "lyr/aglRenderContext.h"

namespace agl::lyr {

class Layer;

class RenderDL : public DisplayListBase {
public:
    ~RenderDL() override;

    // TODO

private:
    Layer* mLayer;
    char _60[0x20];
};

class RenderDLBuffer {
public:
    virtual ~RenderDLBuffer();

private:
    GPUMemBlock<u8> _8[2];
    char _88[8];
    RenderContext _90[3];
    u64 _108;
    char _110[8];
    sead::Buffer<RenderDL> mDisplayLists;
    char _128[0x58];
};

} // namespace agl::lyr