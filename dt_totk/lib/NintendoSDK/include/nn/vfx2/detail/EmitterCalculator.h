#pragma once

#include <nn/vfx2/detail/ResTextureSamplerInfo.h>
#include "nn/vfx2/Resource.h"

namespace nn::vfx2{

class System;

namespace detail {

class TextureSampler;

class EmitterCalculator {
public:
    explicit EmitterCalculator(System*);

private:
    TextureSampler* mSampler0;
    TextureSampler* mSampler1;
    ResTextureSamplerInfo mSamplerInfo0;
    ResTextureSamplerInfo mSamplerInfo1;
};

} // namespace detail

} // namespace nn::vfx2