#pragma once

namespace nn::vfx2 {

class EmitterSet;

struct Handle {
    EmitterSet* mEmitterSet;
    int mIndex;
};

} // namespace nn::vfx2