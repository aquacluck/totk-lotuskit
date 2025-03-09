#pragma once

#include <nn/gfx/gfx_State.h>
#include <nn/gfx/gfx_Types.h>
#include <nn/types.h>
#include <nn/util/MathTypes.h>
#include <nn/vfx2/detail/RenderContext.h>
#include "nn/gfx/gfx_StateInfo.h"
#include "nn/vfx2/Resource.h"
#include "nn/vfx2/detail/BinaryData.h"
#include "nn/vfx2/detail/ShaderManager.h"

namespace nn {

namespace gfx {
class BufferInfo;
struct ResTextureData;
}  // namespace gfx

namespace vfx2 {

namespace detail {
struct BinaryData;
struct ResEmitter;
struct ResEP;  // related to the stripe system, EP might be extended particle
struct ResFieldRandomPos;
struct ResFieldRandomVel;
struct ResFieldMagnet;
struct ResFieldSpin;
struct ResFieldCollision;
struct ResFieldConvergence;
struct ResFieldPositionAdd;
struct ResFieldCurlNoise;
struct ResFieldGWD;  // handled by a callback that's always null, wtf is gwd
struct ResFieldCSF;  // same with this one
struct ResAnimEmitterKeyParamSet;
class ConstantBuffer;
class ComputeShader;
class ComputeShaderManager;
class Shader;
class ShaderManager;
class TextureSampler;
class TextureSamplerManager;
}  // namespace detail

class Primitive;
class Resource;

class EmitterResource {
public:
    EmitterResource();

    void ResolveBinaryData(detail::BinaryData*);

    bool Setup(Resource*, u64, u64);

    bool CreateVertexState(gfx::VertexStateInfo*, gfx::VertexAttributeStateInfo*, gfx::BufferInfo*,
                           u8);

    bool InitializeConstantBuffer(detail::ConstantBuffer*);
    void InitializeRenderState(gfx::TDevice<gfx::DefaultApi>*);
    bool InitializeVertexState(gfx::TDevice<gfx::DefaultApi>*);

    void UpdateParams(Resource*, detail::TextureSamplerManager*);
    bool UpdateShaderResource(detail::ShaderManager* shader_mgr,
                              detail::ComputeShaderManager* compute_shader_mgr,
                              void (*resolve_shader)(void*, detail::Shader*&),
                              void (*resolve_compute_shader)(void*, detail::Shader*&));

    // probably doesn't belong here
    static bool RegisterSystemCallbacks(void* callbacks, EmitterResource*,
                                        void* system_callback_struct);

private:
    detail::ResEmitter* mResEmitter;
    Primitive* mTrimmingPrimitive;
    detail::Shader* mShaderDefault;
    detail::Shader* mShaderPass1;
    detail::Shader* mShaderPass2;
    detail::ComputeShader* mComputeShaderDefault;
    detail::ComputeShader* mComputeShaderPass1;
    detail::ComputeShader* mComputeShaderPass2;
    detail::ConstantBuffer* mConstantBuffer;
    gfx::ResTextureData* mTextureData[6];
    detail::TextureSampler* mTextureSamplers[6];
    u8 mVertexAttrIndexArray[13];
    u8 mSubSectionCount;
    u8 mExtendedParticleType;  // 3 = super stripe, 2 = stripe, 1 = none?
    char _B7[3];
    u16 mSectionIndex;
    u8 _BC;
    u8 mIsSetup;
    bool mHasBehaviorFields;
    u8 _BF;
    int mMaxLifespan;
    float mMaxEmissionRate;
    gfx::VertexState mVertexState[3];
    detail::ResEP* mResEP;
    char _130[0x10];
    float _140[28];
    util::Float4 mRotation;
    detail::RenderContext mRenderContext;
    void* mEmitterData;  // ResEmitter + 0x70 (immediately following the name)
    Primitive* mPrimitive;
    EmitterResource* mChildEmitters[17];
    detail::ResFieldRandomPos* mRandomPosField;
    detail::ResFieldRandomVel* mRandomVelField;
    detail::ResFieldMagnet* mMagnetField;
    detail::ResFieldSpin* mSpinField;
    detail::ResFieldCollision* mCollisionField;
    detail::ResFieldConvergence* mConvergenceField;
    detail::ResFieldPositionAdd* mPositionAddField;
    detail::ResFieldCurlNoise* mCurlNoiseField;
    detail::ResFieldGWD* mGWDField;
    detail::ResFieldCSF* mCSFField;
    detail::ResAnimEmitterKeyParamSet* mESAnim;            // what is ES? EAES
    detail::ResAnimEmitterKeyParamSet* mRotationAnim;      // EAER
    detail::ResAnimEmitterKeyParamSet* mETAnim;            // what is ET? translation? time? EAET
    detail::ResAnimEmitterKeyParamSet* mColor0Anim;        // EAC0
    detail::ResAnimEmitterKeyParamSet* mColor1Anim;        // EAC1
    detail::ResAnimEmitterKeyParamSet* mEmissionRateAnim;  // unsure EATR
    detail::ResAnimEmitterKeyParamSet* mParticleLifespanAnim;  // EAPL
    detail::ResAnimEmitterKeyParamSet* mAlpha0Anim;            // EAA0
    detail::ResAnimEmitterKeyParamSet* mAlpha1Anim;            // EAA1
    detail::ResAnimEmitterKeyParamSet* mObjectVelAnim;  // seems to be for individual particles EAOV
    detail::ResAnimEmitterKeyParamSet*
        mDirectionalVelAnim;                        // seems to be for the entire emitter EADV
    detail::ResAnimEmitterKeyParamSet* mScaleAnim;  // seems to be for individual particles EASL
    detail::ResAnimEmitterKeyParamSet*
        mSizeScaleAnim;                          // seems to be for the entire emitter volume EASS
    detail::ResAnimEmitterKeyParamSet* mGVAnim;  // what is GV? EAGV
    void* mCSDP;
    void* mCADP;
    void* mCUDP;
    void* mVertexStateMemory[3];
    int mBufferBaseOffset;
    int mBufferEPOffset;
    int mBufferCSDPOffset;
    int mBufferFROffset;
    u32 mCSDPSize;
};

}  // namespace vfx2
}  // namespace nn