#pragma once

#include <nn/gfx/gfx_Common.h>
#include <nn/gfx/gfx_Enum.h>
#include <nn/gfx/gfx_Types.h>
#include "nn/gfx/detail/gfx_Fwd.h"
#include <nn/nn_BitTypes.h>
#include <nn/util/util_BitPack.h>

namespace nn {

namespace gfx {
class ResShaderFile;
class ResShaderContainer;
class ShaderInfo;
}

namespace vfx2 {

class Heap;

namespace detail {
class Shader;
class ComputeShader;
struct ResShaderInfo;

class ShaderManager {
public:
    bool Initialize(gfx::detail::DeviceImpl<gfx::DefaultApi>, Heap*, gfx::ResShaderFile*, bool);
    
    void InitializeShader(Shader*, gfx::ShaderInfo*);
    void InitializeShader2(Shader*, gfx::ShaderInfo*);

private:
    char _00[8];
    gfx::Device* mDevice;
    gfx::ResShaderContainer* mShaderContainer;
    Shader* mShaders;
    ResShaderInfo* mStaticShaders;
    ResShaderInfo* mShaderInfo;
    gfx::ShaderCodeType mShaderCodeType;
    int mShaderStageCount;
    Bit8 state;
    nn::util::BitPack8 flags;
};

class ComputeShaderManager {
public:
    bool Initialize(gfx::detail::DeviceImpl<gfx::DefaultApi>, Heap*, gfx::ResShaderFile*, bool);
    
    void InitializeShader(ComputeShader*, gfx::ShaderInfo*);
    void InitializeShader2(ComputeShader*, gfx::ShaderInfo*);

private:
    char _00[8];
    gfx::Device* mDevice;
    gfx::ResShaderContainer* mShaderContainer;
    ComputeShader* mShaders;
    ResShaderInfo* mStaticShaders;
    ResShaderInfo* mShaderInfo;
    gfx::ShaderCodeType mShaderCodeType;
    int mShaderStageCount;
    Bit8 state;
    nn::util::BitPack8 flags;
};

}

} // namespace vfx2

} // namespace nn