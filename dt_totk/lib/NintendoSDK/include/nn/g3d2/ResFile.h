/**
 * @file ResFile.h
 * @brief Resource file for models.
 */

#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>
#include <nn/util.h>
#include <nn/util/util_BinaryFormat.h>
#include "nn/g3d2/ResSkeletalAnim.h"

namespace nn {

namespace gfx {
template <typename T>
class TDevice;
class MemoryPoolInfo;
class ResEmbedFile;
}

namespace util {
class ResDic;
}

namespace g3d2 {
class ResModel;
class ResSkeletalAnim;
class ResMaterialAnim;
class ResBoneVisibiltiyAnim;
class ResShapeAnim;
class ResSceneAnim;
typedef void* TextureRef;

class ResFile : public nn::util::BinaryFileHeader {
public:
    static bool IsValid(void const* modelSrc);
    void Relocate();
    void Unrelocate();
    static ResFile* ResCast(void*);
    s32 BindTexture(TextureRef (*ref)(char const*, void*), void*);
    void ReleaseTexture();
    void Setup(gfx::Device*);
    void Setup(gfx::Device*, gfx::MemoryPool*, s64, u64);
    void Cleanup(gfx::Device*);
    void Reset();

    char* mFileName;
    ResModel* mModels;
    util::ResDic* mModelDict;
    void* _38[4];
    ResSkeletalAnim* mSkeletalAnims;
    util::ResDic* mSkeletalAnimDict;
    ResMaterialAnim* mMaterialAnims;
    util::ResDic* mMaterialAnimDict;
    ResBoneVisibiltiyAnim* mBoneVisAnims;
    util::ResDic* mBoneVisAnimDict;
    ResShapeAnim* mShapeAnims;
    util::ResDic* mShapeAnimDict;
    ResSceneAnim* mSceneAnims;
    util::ResDic* mSceneAnimDict;
    gfx::MemoryPool* mMemoryPool;
    gfx::MemoryPoolInfo* mMemoryPoolInfo;
    gfx::ResEmbedFile* mEmbeddedFiles;
    util::ResDic* mEmbeddedFileDict;
    void* _C8;
    char* _D0;
    int _D8;
    u16 mModelCount;
    u16 _DE;
    u16 _E0;
    u16 mSkeletalAnimCount;
    u16 mMaterialAnimCount;
    u16 mBoneVisAnimCount;
    u16 mShapeAnimCount;
    u16 mSceneAnimCount;
    u16 mEmbeddedFileCount;
    u16 _EE;
};

}  // namespace g3d2
}  // namespace nn
