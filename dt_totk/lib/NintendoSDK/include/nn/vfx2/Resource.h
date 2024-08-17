#pragma once

#include <nn/g3d2/ResFile.h>
#include <nn/gfx/gfx_DescriptorSlot.h>
#include <nn/gfx/gfx_Device.h>
#include <nn/gfx/gfx_ResShader.h>
#include <nn/gfx/gfx_ResTexture.h>
#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>
#include <nn/vfx2/Heap.h>
#include <nn/vfx2/detail/ConstantBuffer.h>
#include <nn/vfx2/detail/ShaderManager.h>
#include "nn/gfx/gfx_Common.h"

namespace nn {

namespace gfx {
struct ResTextureData;
}  // namespace gfx

namespace g3d2 {
class ResFile;
}  // namespace g3d2

namespace util {
struct BinaryFileHeader;
}  // namespace util

namespace vfx2 {

class Heap;
class EmitterResource;
class EmitterSetResource;
class Primitive;
class G3dResPrimitive;
class System;

namespace detail {
struct BinaryData;
struct BufferSizeCalculator;
struct ResNameTableRecord;
struct ResG3dPrimitiveTableRecord;
class Heap;
class TextureSamplerManager;
}  // namespace detail

class Resource {
public:
    Resource();
    virtual ~Resource();

    // I feel like I remember there being a nn::vfx2::Result type or something but I can't find it anymore
    bool Initialize(gfx::TDevice<gfx::DefaultApi>*, detail::Heap*, void*, int,
                    detail::TextureSamplerManager*, void*, void*, void*);
    bool Initialize(gfx::TDevice<gfx::DefaultApi>*, detail::Heap*, void*, gfx::MemoryPool*, void*,
                    void*, int, detail::TextureSamplerManager*, u8, void*, Resource*, void*);

    bool Trace(detail::BinaryData*, bool, void*);
    void Finalize(Heap*, void*);
    bool TraceGfxResTextureFile(detail::BinaryData*);
    bool TracePrimitiveArray(detail::BinaryData*);
    bool TraceTrimmingPrimitiveArray(detail::BinaryData*);
    bool TraceG3dPrimitiveArray(detail::BinaryData*);
    bool TraceShaderBinaryArray(detail::BinaryData*);
    bool TraceEmitterSetArray(detail::BinaryData*, detail::BufferSizeCalculator*);
    bool InitializeEmitterGraphicsResource(EmitterResource*);
    void FinalizeEmitterResource(EmitterResource*);
    void Unrelocate(void*);
    void RegisterTextureViewToDescriptorPool(bool (*)(gfx::DescriptorSlot*, const gfx::TextureView&,
                                                      void*),
                                             void*);
    bool GetTextureDescriptorSlot(gfx::DescriptorSlot*, size_t) const;
    gfx::ResTextureData* GetResTextureData(size_t) const;
    void SearchRecordFromResNameTable(detail::ResNameTableRecord*, size_t);
    void BindExternalResTextureFile(gfx::ResTextureFile*);
    void UnregisterTextureViewFromDescriptorPool(void (*)(gfx::DescriptorSlot*,
                                                          const gfx::TextureView&, void*),
                                                 void*);
    void BindExternalG3dResFile(g3d2::ResFile*);
    void BindExternalResShaderFile(gfx::ResShaderFile*, gfx::ResShaderFile*);
    void InitializeEmitterSetResource(EmitterSetResource*, detail::BinaryData*,
                                      detail::BufferSizeCalculator*);
    void InitializeEmitterResource(EmitterResource*, detail::BinaryData*, int,
                                   detail::BufferSizeCalculator*);
    Primitive* GetPrimitive(size_t) const;
    Primitive* GetTrimmingPrimitive(size_t) const;
    G3dResPrimitive* GetG3dPrimitive(size_t) const;
    int SearchEmitterSetId(const char*) const;
    int SearchEmitterSetIdWithFilePath(const char*) const;
    void BindResource(int, EmitterSetResource*);
    void UnbindResource(int);
    bool IsExistChildEmitter(int) const;
    bool IsNeedFade(int) const;
    void OutputResourceInfo() const;

    bool Verify(util::BinaryFileHeader*) const;

private:
    detail::ConstantBuffer mConstantBuffer;
    detail::ConstantBuffer mTrimmingPrimitiveConstantBuffer;
    ResHeap mResHeap;
    detail::ShaderManager mShaderManager;
    detail::ComputeShaderManager mComputeShaderManager;
    gfx::Device* mDevice;
    gfx::MemoryPool* mMemoryPool;
    EmitterSetResource* mEmitterSetResources;
    char _438[8];
    util::BinaryFileHeader* mFile;
    void* mPrimitiveMemory;
    Primitive* mPrimitives;
    void* mTrimmingPrimitiveMemory;
    Primitive* mTrimmingPrimitives;
    void* mG3dPrimitiveMemory;
    G3dResPrimitive* mG3dPrimitives;
    gfx::ResTextureFile* mResTextureFile;
    void* mGTNTSection;
    detail::ResG3dPrimitiveTableRecord* mResG3dPrimitiveTableRecord;
    Resource* _490;
    char _498[0x18];
    detail::TextureSamplerManager* mTextureSamplerManager;
    void* _4B8;  // some kind of system callback struct
    g3d2::ResFile* mG3dResFile;
    char _4C8[0x10];
    size_t mEmitterSetDataSize;
    size_t mTextureDataSize;
    size_t mBinaryShaderDataSize;
    char _4F0[8];
    size_t mPrimitiveDataSize;
    size_t mTrimmingPrimitiveDataSize;
    size_t mG3dPrimitiveDataSize;
    size_t mBufferSize;
    size_t mTextureSize;
    size_t mPrimitiveArraySize;
    size_t mTrimmingPrimitiveArraySize;
    size_t mShaderBinarySize;
    size_t mG3dPrimitiveSize;
    size_t mEmitterSetArraySize;
    size_t mVertexStateSize;
    int mActiveEmitterSetCount;
    int _554;
    int mEmitterSetCount;
    int mEmitterCount;
    int mPrimitiveCount;
    int mTrimmingPrimitiveCount;
    int mEffectModelCount;
    int mTextureCount;
    bool mInitializedShaders;
    u8 _571;
};
static_assert(sizeof(Resource) == 0x578);

}  // namespace vfx2
}  // namespace nn
