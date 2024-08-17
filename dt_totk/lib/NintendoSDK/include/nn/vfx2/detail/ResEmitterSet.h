#pragma once

namespace nn::vfx2::detail {

struct ResEmitterSet {
    char _00[0x10];
    char mName[0x60];
    int _70[9];
    float _94[4];
    int _A4;
    char _A8;
    int _AC[2]; 
};

} // namespace nn::vfx2::detail