#pragma once

#include <cstddef>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <string>

#include "exlaunch.hpp"

namespace havok {
    class hknpBody;
    class hknpMotion;
    class hknpShape;
    class hknpWorld;
    class hknpWorldWriter { virtual ~hknpWorldWriter(); };
    class hknpWorldReader { virtual ~hknpWorldReader(); };
    class hknpBodyManager;
    class hknpMotionManager;
    class hknpConstraintManager;
    class hknpModifierManager;
    class hkReferencedObject;

    class hknpBody {
        public:
        float hkRotation[12]; // hknpPreciseTransform
        sead::Vector4f translation;
        sead::Vector4f translationAdjustment;
        u32 serialAndIndex; // hknpBodyIndex
        u32 flags;
        u16 collisionControl;
        u16 timAngle;
        u16 maxTimDistance;
        u16 maxTimDistanceFromRotation;
        float aabb[6];
        hknpShape* shape; // always present?
        u32 motionId;
        u32 nextAttachedBodySerialAndIndex; // hknpBodyIndex
        u8 addedToWorldFlags;
        u8 qualityId;
        u16 materialId;
        u32 collisionFilterInfo;
        u16 maxContactDistance;
        u16 maxContactDistanceFromRotation;
        u16 radiusOfComCenteredBoundingSphere; // half
        u16 maxDepenetrationSpeed; // half
        u8 activationPriority; u8 _[7];
        sead::Quatf bodyFromMotionRotation;
        u64 bodyId;
        u64 userData;

        sead::BoundBox3f getAABB() {
            auto pos = (sead::Vector3f*)(&translation);
            auto min = sead::Vector3f(aabb[4], aabb[5], aabb[3]) - *pos;
            auto max = sead::Vector3f(aabb[0], aabb[1], aabb[2]) - *pos;
            return sead::BoundBox3f(min, max);
        }
    };
    static_assert(sizeof(hknpBody) == 0xc0);
    static_assert(offsetof(hknpBody, translation) == 0x30);
    static_assert(offsetof(hknpBody, aabb) == 0x60);
    static_assert(offsetof(hknpBody, shape) == 0x78);

    class hknpMotion {
        public:
        sead::Quatf orientation;
        sead::Vector3<double> centerOfMass;
        sead::Vector3f previousStepLinearVelocity; // hkFloat3Pair
        sead::Vector3f previousStepAngularVelocity;
        f16 inverseInertia[4];
        u32 firstAttachedBodyIndex; // hknpBodyIndex
        u32 solverId;
        u8 _0[6];
        f16 integrationFactor;
        u16 motionPropertiesId;
        f16 lookAheadDistance;
        f16 maxRotationPerStep;
        u8 cellIndex;
        u8 spaceSplitterWeight;
        sead::Vector4f linearVelocityAndSpeedLimit;
        sead::Vector4f angularVelocityLocalAndSpeedLimit;
    };
    static_assert(sizeof(hknpMotion) == 0x80);

    class hkReferencedObject {
        virtual ~hkReferencedObject(); // vtable
        public:
        u64 sizeAndFlags;
        u64 refCount;
    };
    static_assert(sizeof(hkReferencedObject) == 0x18);

    class hknpShape: public hkReferencedObject {
        virtual ~hknpShape(); // vtable
        public:
        u8 type;
        u8 dispatchType;
        u16 flags;
        u8 numShapeKeyBits;
        u8 idk[3];
        float convexRadius;
        u8 idk2[4];
        u64 userData; // always present?
        void** properties; // hknpShapeProperties
        u32 propertiesSize;
        u32 propertiesCapacityAndFlags;
    };
    static_assert(sizeof(hknpShape) == 0x40);

    class __attribute__((packed)) hknpBodyBuffer {
        public:
        hknpBody* mData; // hkArray<hknpBody>
        u32 mCapacity;
        u32 mSizeAndFlags;
        u64 idk0;
        s32 idk1;
        u32 idk2;
        s32 idk3;
        u64 mAllocatedCount;
        s32 idk4;
    };
    static_assert(sizeof(hknpBodyBuffer) == 0x30);

    class hknpBodyManager {
        public:
        hknpWorld* mpWorld;
        hknpMotionManager* mpMotionManager;
        hknpBodyBuffer mBodyBuffer;
        u8 _0[0xd8];
    };
    static_assert(sizeof(hknpBodyManager) == 0x118);

    class UserMotionBufferStruct1 {
        public:
        hknpMotion* mData;
        s32 mCapacity;
        s32 mSizeAndFlags;
        u64 idk[4];
    };
    static_assert(sizeof(UserMotionBufferStruct1) == 0x30);

    class UserMotionBufferStruct {
        public:
        u32* mData;
        s32 mSize; // XXX doesn't match others?
        s32 mCapacityAndFlags;
        u8 idk[8];
    };
    static_assert(sizeof(UserMotionBufferStruct) == 0x18);

    class hknpMotionManager {
        public:
        hknpBodyManager* mpBodyManager;
        void* mpDeactivationManager;
        UserMotionBufferStruct1 userStruct;
        UserMotionBufferStruct userStructMore[32];
        void* mpCS;
    };
    static_assert(sizeof(hknpMotionManager) == 0x348);

    class hknpWorld {
        virtual ~hknpWorld(); // vtable
        public:
        u64 mSizeAndFlags;
        u64 mRefCount;
        hknpWorldWriter mWorldWriter;
        hknpWorldReader mWorldReader;
        hknpBodyManager mBodyManager;
        hknpMotionManager mMotionManager;
        hknpConstraintManager* mConstraintManager;
        hknpModifierManager* mModifierManager;
        void* mParticlesColliderManager;
        u8 _0[0x530];
    };
    static_assert(offsetof(hknpWorld, mBodyManager) == 0x28);
    static_assert(offsetof(hknpWorld, mMotionManager) == 0x140);
    static_assert(sizeof(hknpWorld) == 0x9d0);

} // ns

