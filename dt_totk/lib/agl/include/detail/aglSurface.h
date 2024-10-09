#pragma once

#include <nvn/nvn.h>
#include <basis/seadTypes.h>
#include "common/aglTextureEnum.h"
#include "prim/seadSizedEnum.h"

namespace agl::detail {

struct SurfaceBase;

class Surface {
public:
    Surface();
    void initialize(TextureType, TextureFormat, u32, TextureAttribute, MultiSampleType);
    void initializeSize(u32, u32, u32);
    void copyFrom(const SurfaceBase&);
    void calcSizeAndAlignment();
    void setupNVNtextureBuilder(NVNtextureBuilder*) const;
    void printInfo() const;
    void copyFrom(const NVNtexture&);

private:
    u16 mWidth;
    u16 mHeight;
    u16 mDepth;
    sead::SizedEnum<TextureFormat, u16> mFormat;
    u8 mSamples;
    u8 mLevels;
    u16 mTarget;
    size_t mStorageSize;
    int mStorageAlignment;
    int _1C;
    u8 mNvnSwizzle[4];
    u16 mFlags;
    u16 mStride;
};

struct SurfaceBase {
    void* _0;
    void* _8;
    void* _10;
};

}  // namespace agl::detail
