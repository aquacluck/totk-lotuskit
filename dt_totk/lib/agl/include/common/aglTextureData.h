#pragma once

#include <nn/gfx/gfx_DescriptorSlot.h>
#include <nn/gfx/gfx_Types.h>
#include <prim/seadSafeString.h>
#include "common/aglGPUMemAddr.h"
#include "detail/aglSurface.h"
#include "driver/aglNVNtexture.h"

namespace agl {

class TextureData {
public:
    class CompressToWork {
    public:
        explicit CompressToWork(const TextureData&);

        void* _0;
        void* _8;
        void* _10;
        void* _18;
        u32 _20;
        void* _28;
        void* _30;
        u32 _38;
        void* _40;
        detail::Surface mSurface;
        driver::NVNtexture_ mTexture;
    };

    TextureData();

    void setMipLevelNum_(s32, bool);
    u16 getMinSlice_() const;
    void getTextureFormatName() const;
    u32 calcMipByteSize(u32) const;
    bool isCompressedFormat() const;
    bool isRenderTargetCompressAvailable() const;
    bool isDepthFormat() const;
    bool hasStencil() const;
    void invalidateCPUCache();
    void flushCPUCache() const;
    void setDebugLabel(const sead::SafeString& debug_label);
    void getDebugLabel() const;

private:
    GPUMemAddr<u8> mGpuMemAddr[2];
    detail::Surface mSurface;
    driver::NVNtexture_ mTexture;
    char _68[0x20];
    nn::gfx::TextureView* mTextureView;
    nn::gfx::DescriptorSlot mDescriptorSlot;
    char _98[8];
    sead::SizedEnum<TextureFormat, u8> mImageFormat;
    u8 mState;
    u8 _A2;
    const char* mDebugLabel;  // "agl::TextureData string"
};

}  // namespace agl
