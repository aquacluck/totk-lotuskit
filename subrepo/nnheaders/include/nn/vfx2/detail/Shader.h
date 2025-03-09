#pragma once

#include <nn/gfx/gfx_Enum.h>
#include <nn/gfx/gfx_Types.h>
#include <nn/nn_BitTypes.h>
#include <nn/types.h>

namespace nn {

namespace gfx {
class ResShaderVariation;
} // namespace gfx

namespace vfx2::detail {

class Slot {
public:
    void Initialize();
    void Invalidate();

private:
    u8 mSlot;
};

struct ShaderInterfaceSlots {
    Slot mSysTextureSamplers[6];
    Slot mSysFrameBufferTexture;
    Slot mSysDepthBufferTexture;
    Slot mSysCurlNoiseTextureArray;
    Slot mSysCustomShaderTextureSampler[16];
    Slot mSysCustomShaderTextureArraySampler[4];
    Slot mSysCustomShaderTexture3DSampler[4];
    Slot mSysCustomShaderCubeSampler[4];
    Slot mSysCustomShaderCubeArraySampler[4];
    Slot mSysCustomShaderShadowSampler[4];
    Slot mSysCustomShaderShadowArraySampler[4];
    
    Slot mNnVfx2ViewParam;
    Slot mSysEmitterStaticUniformBlock;
    Slot mNnVfx2EmitterDynamicParam;
    Slot mSysEmitterFieldUniformBlock;
    Slot mSysEmitterPluginUniformBlock;
    Slot mSysCustomShaderReservedUniformBlockParam;
    Slot mSysCustomShaderUniformBlock[6];
    Slot mSysEmitterPluginUnorderedAccessBuffer;
    Slot mSysParticleCustomAttribute[4];
};

// does this exist? where else does GetInterfaceSlot go though?
class ShaderBase {
public:
    static bool GetInterfaceSlot(Slot*, gfx::ResShaderVariation*, gfx::ShaderStage, gfx::ShaderInterfaceType, char*);

private:
    gfx::Shader* mShaderVariation;
};

class Shader : ShaderBase {
public:
    void InitializeShader(gfx::Shader*);
    void Initialize();
    void Invalidate();

private:
    ShaderInterfaceSlots mVertexShaderSlots;
    ShaderInterfaceSlots mPixelShaderSlots;
    bool mState;
};

class ComputeShader : ShaderBase {
public:
    void InitializeShader(gfx::Shader*);
    void Initialize();
    void Invalidate();

private:
    Slot mSysPosBuff;
    Slot mSysPosBuffSlot;
    Slot mSysPosDeltaBuff;
    Slot mSysScaleBuff;
    Slot mSysRandomBuff;
    Slot mSysEmtMat0;
    Slot mSysEmtMat1;
    Slot mSysEmtMat2;
    Slot mSysDefaultCustomAttrBuff;
    ShaderInterfaceSlots mShaderSlots;
    bool mState;
};

} // namespace vfx2::detail

} // namespace nn