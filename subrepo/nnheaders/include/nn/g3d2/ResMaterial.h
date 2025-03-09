/**
 * @file ResMaterial.h
 * @brief Resource material for models.
 */

#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/nn_BitTypes.h>
#include <nn/types.h>

namespace nn {

namespace gfx {
class DescriptorSlot;
class SamplerInfo;
class UserData;
}

namespace util {
class ResDic;
}

namespace g3d2 {
class ResShaderData;
typedef void* TextureRef;

class ResMaterial {
public:
    u64 BindTexture(nn::g3d2::TextureRef (*)(char const*, void*), void*);
    void ForceBindTexture(nn::g3d2::TextureRef const&, char const*);
    void ReleaseTexture();
    void Setup(gfx::Device*);
    void Cleanup(gfx::Device*);
    void Reset();
    void Reset(u32);

    char mMagic[4];
    u32 mIsVisible;
    char* mName;
    ResShaderData* mShaderData;
    gfx::TextureView** mTextureViewArray;
    char** mTextureNameArray;
    gfx::Sampler* mSamplers;
    gfx::SamplerInfo* mSamplerInfo;
    util::ResDic* mSamplerDict;
    void* mRenderInfoValues;
    u16* mRenderInfoValueCounts;
    u16* mRenderInfoValueOffsets;
    void* mShaderParamValues;
    int* mShaderParamUBOOffsets;
    void* _68;
    gfx::UserData* mUserData;
    util::ResDic* mUserDataDict;
    Bit32* mShaderParamConvertFlags;
    void* mRuntimeUser;
    gfx::DescriptorSlot* mSamplerDescriptors;
    gfx::DescriptorSlot* mTextureDescriptors;
    u16 mIndex;
    u8 mSamplerCount;
    u8 mTextureCount;
    u16 _A4;
    u16 mUserDataCount;
    u16 _A8;
    u16 mShadingModelOptionUBOSize;
    u32 _AC;
};

}  // namespace g3d2
}  // namespace nn
