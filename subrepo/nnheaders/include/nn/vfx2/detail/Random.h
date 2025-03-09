#pragma once

#include <nn/types.h>
#include <nn/util/MathTypes.h>

namespace nn::vfx2::detail {

class Random {
public:
    Random();

    void Initialize();
    void Finalize();

    void SetSeed(u32);

    static util::Float3* g_Vec3fTable;
    static util::Float3* g_NormalizedVec3fTable;

    static bool g_UseGlobalSeed;
    static u32 g_GlobalSeed;

    static Random g_Random;

    static Random* GetGlobalRandom() { return &g_Random; } 

private:
    u16 mX;
    u16 mY;
    u32 mZ;
};

} // namespace nn::vfx2::detail