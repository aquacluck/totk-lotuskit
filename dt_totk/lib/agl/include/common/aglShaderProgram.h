#pragma once

#include <container/seadBuffer.h>
#include <prim/seadSafeString.h>
#include <nn/gfx/gfx_BufferInfo.h>
#include "common/aglDisplayList.h"
#include "common/aglShader.h"

namespace sead {
class Heap;
}

namespace agl {

class ShaderProgram {
public:
    class VariationBuffer {
    public:
        void initialize(s32, sead::Heap*);
        void createMacro(s32, const sead::SafeString&, const sead::SafeString&, s32, sead::Heap*);
        void setMacroValue(s32, s32, const sead::SafeString&);
        void create(sead::Heap*);
    };

    ShaderProgram();

    virtual ~ShaderProgram();

    void cleanUp();
    void destroyLocationBuffers();
    void initializeVariation(const sead::SafeString&, s32, sead::Heap*);
    void createVariationMacro(s32, const sead::SafeString&, const sead::SafeString&, s32,
                              sead::Heap*);
    void setVariationMacroValue(s32, s32, const sead::SafeString&);
    void createVariation(sead::Heap*);

private:
    VariationBuffer* mVariation;
    DisplayListBase mShaderDisplayList;  // _10
    char _10[0x10];
    u8 _20;
    u8 _21;
    u16 mVariationIndex;
    u16 _24;
    u16 _26;
    char _28[0x20];
    sead::Buffer<nn::gfx::BufferInfo> mUniformBlockArray;
    sead::Buffer<nn::gfx::BufferInfo> mSSBOArray;
    sead::Buffer<nn::gfx::BufferInfo> mSamplerLocationArray;
    char _78[0xd0];
    u64 mBufferAddress;
    char _150[8];
};

}  // namespace agl
