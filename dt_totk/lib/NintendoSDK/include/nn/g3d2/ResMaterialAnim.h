/**
 * @file ResMaterialAnim.h
 * @brief Resource file for material animations.
 */

#pragma once

namespace nn {
namespace g3d2 {
typedef void* TextureRef;

class ResMaterialAnim {
public:
    void ReleaseTexture();
    s32 BindTexture(nn::g3d2::TextureRef (*)(char const*, void*), void*);
    void Reset();
};
}  // namespace g3d2
}  // namespace nn
