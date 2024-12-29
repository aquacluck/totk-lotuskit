#pragma once

#include "structs/bbBlackboard.hpp"
#include "structs/phive.hpp"

#include <cstddef>
#include <container/seadPtrArray.h>
#include <container/seadRingBuffer.h>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>
#include <prim/seadTypedBitFlag.h>
#include <prim/seadSafeString.h>
#include <thread/seadCriticalSection.h>

namespace engine::actor {
    class ActorBaseLink;
} // namespace engine::actor

//using DtorFunc = void (engine::actor::ActorBaseLink*);
//extern DtorFunc* ActorLinkDtor;

namespace sead {
    class ListNode;
    class Function;
    class Heap;
} // namespace sead

namespace engine::transceiver {
    class Transceiver {
        virtual ~Transceiver(); // for the vtable

        protected:
        u32 mIndex;
        u32 mChannelMask;
        char mMapNode[0x38]; // whatever
    };
} // namespace transceiver

namespace engine::component {
    //class IActorComponent; // XXX is this relevant as a category?
    class Component {
        public:
        virtual ~Component(); // vtable i guess
        sead::SafeString refPath;
    };

    class PhysicsComponent: public Component {
        public:
        u8 idkman[0x10];
        phive::ControllerSet* controllerSet;
    };
    static_assert(offsetof(PhysicsComponent, controllerSet) == 0x20);

    class Model { // XXX ns?
        public:
        char _00[0x1f8];
        sead::Matrix34f modelMtx;
    };
    class ModelComponent : public Component {
        public:
        char _10[0x18];
        Model* model;
    };
    static_assert(offsetof(ModelComponent, model) == 0x28);

} // namespace component

namespace engine::actor {
    enum class CreatePriority : u32 {
        Highest, High, Middle, Low, Lowest
    };

    // I think it's actually a sead enum but whatever
    enum class CalcPriority : u32 {
        Before, Default, After
    };

    class ActorBase;
    class ActorFile;
    class ActorMgr;
    class BaseProcLinkData;
    class PreActor;

    struct CreateWatcher {
        u32 status;
        ActorBase* actor;
        sead::CriticalSection cs;
    };
    static_assert(sizeof(CreateWatcher) == 0x30);

    struct CreateWatcherRef {
        CreateWatcher* watcher;
    };

    class BaseProc {
        public:
        // just so there's a vtable, the vtable is huge so I'm not filling it in here
        virtual void checkDerivedRuntimeTypeInfo(void*) const;
        virtual void* getRuntimeTypeInfo() const;

        using BaseProcId = u32;
        static constexpr BaseProcId cInvalidId = 0xffffffff;

        enum State : u32 {
            Uninitialized, Initializing, PostInitialized, Schedulable, Calc, Paused, Sleeping,
            PreFinalize, Finalizing, PostFinalize, ScheduledForDeletion, Deleted,
        };

        //protected:
        sead::Heap* mInstanceHeap;
        BaseProcId mID;
        sead::SafeString mName;
        CalcPriority mPriority;
        State mState;
        u32 mFlags;
        u32 mMgrListFlags;
        void* mJobLinks[4][8]; // too lazy to make the struct
        sead::ListNode* mJobListNode;
        char mMapNode[0x50]; // should be sead::TTreeNode<BaseProc*> or something but that doesn't line up exactly
        void* _188;
        void* _190;
        void* _198;
        void* _1a0;
        BaseProcLinkData* mLinkData;
        u32 mRefCount; // atomic
        CreateWatcherRef mWatcher;
        sead::FixedRingBuffer<u16, TOTK_VERSION == 100 ? 29 : 33> mDependencyRing;
    };
    static_assert(sizeof(BaseProc) == (TOTK_VERSION == 100 ? 0x210 : 0x218));

    class ActorTransceiver : public transceiver::Transceiver {
        public:
        ~ActorTransceiver() override;

        private:
        ActorBase* mActor;
    };
    static_assert(sizeof(ActorTransceiver) == 0x50);

    class BaseProcLink {
        public:
        virtual ~BaseProcLink(); // vtable i guess
        BaseProcLinkData* mLinkData;
        BaseProc::BaseProcId mID = BaseProc::cInvalidId;
        u8 _14;
        u8 _15;
        u8 mRefMask;
        u8 _17;
    };

    class BaseProcLinkData {
        public:
        char readCS[0x20];
        char writeCS[0x20];
        ActorBase* baseProc;
        int baseProcId;
        u64 refMask;
        u8 _58;
    };
    static_assert(sizeof(BaseProcLinkData) == 0x60);

    class ActorBaseLink: public BaseProcLink {
        public:
        virtual void checkDerivedRuntimeTypeInfo(void*) const;
        virtual void* getRuntimeTypeInfo() const;
        //virtual ~ActorBaseLink() { ActorLinkDtor(this); };
    };
    static_assert(sizeof(ActorBaseLink) == 0x18);

    // unsure if this exists
    struct CreateWatcherLink {
        ActorBaseLink actor_link;
        CreateWatcherRef watcher;
    };

    struct LoadCache {
        virtual ~LoadCache();

        struct Entry {
            sead::SafeString file_path;
            sead::SafeString classname;
        };

        sead::Buffer<u64> mPathHashes; // sead::Buffer<pp::Hash>
        sead::Buffer<Entry> mLoadEntries;
        sead::Buffer<char> mPathNameBuffer;
    };

    class ActorBase : public BaseProc {
        public:
        void checkDerivedRuntimeTypeInfo(void*) const override;
        void* getRuntimeTypeInfo() const override;

        using ActorId = u32;
        static constexpr ActorId cInvalidId = 0xffffffff;

        sead::Vector3f getPosition() const { return mPosition; }

        //protected:
        sead::SafeString mActorName;
        sead::PtrArray<engine::component::Component> mComponents;
        sead::Buffer<u8> mComponentIndexArray;
        sead::Buffer<u8> _240;
        u8 _250;
        void* mPreCalcJob;
        void* mPostCalcJob;
        void* mPostSensorJob;
        void* mFrameEndCalcJob;
        void* mSleepCalcJob;
        float _280;
        sead::Vector3f mHomePos;
        sead::Matrix33f mHomeRotation;
        sead::Vector3f mPosition;
        sead::Matrix33f mRotation;
        sead::Vector3f mScale;
        sead::Vector3f mLastPosition; // XXX "next"?
        sead::Matrix33f mLastRotation; // XXX "next"?
        sead::Vector3f mLastLinearVelocity; // XXX still "last"?
        sead::Vector3f mLastAngularVelocity; // XXX still "last"?
        float _338;
        float _33c;
        float _340;
        float _344;
        float _348;
        float _34c;
        sead::BoundBox3f mAABB;
        void* mActorParamDocument;
        PreActor* mPreActor;
        ActorTransceiver mTransceiver;
        void* mBlackboardAccessor;
        CreateWatcherLink mWatcherLink;
        ActorId mActorID;
        int _3f0;
        u32 _3f8;
        u32 mPauseFlags;
        char mPauseMasks[0x20]; // sead::TreeMap<u32, u32>
        sead::CriticalSection mPauseMaskCS;
        u32 mFlags440;
        u32 mFlags444;
        u64 _448;
        u64 _450;
        sead::CriticalSection _458;
        LoadCache mTLS;
        u8 _4b0;
        u8 _4b1;

        sead::Matrix34f getTransform() const {
            return sead::Matrix34f(mRotation, mPosition);
        }

        engine::component::PhysicsComponent* getPhysicsComponent() const {
            return reinterpret_cast<engine::component::PhysicsComponent*>(mComponents[0xa]);
        }

        engine::component::ModelComponent* getModelComponent() const {
            return reinterpret_cast<engine::component::ModelComponent*>(mComponents[2]);
        }

    };
    static_assert(sizeof(ActorBase) == (TOTK_VERSION == 100 ? 0x4b0 : 0x4b8));
    static_assert(offsetof(ActorBase, mAABB) == (TOTK_VERSION == 100 ? 0x348 : 0x350));

    class BaseProcMgr {
        public:
        // just for create arg (does this actually belong here hmmmmm)
        struct CreateArg {
            enum class TransformFlags : u8 {
                UsePosition = 1 << 0,
                UseRotation = 1 << 1,
                UseScale = 1 << 2,
            };

            sead::Vector3f position{ 0.f, 0.f, 0.f };
            sead::Matrix33f rotation{ 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };
            sead::Vector3f scale{ 1.f, 1.f, 1.f };
            // sead::SafeString defaults to a nullptr instead of a null string so we have to do this
            sead::SafeString _40{""};
            u8 _48;
            u8 _49[0x7];
            bool _50;
            u8 _51;
            u8 _52;
            u8 _53;
            bb::InitInfo<32>* blackboard_info = nullptr;
            bb::InitInfo<32>* _60 = nullptr;
            ActorBase* parent = nullptr;
            ActorBase* dependent = nullptr;
            ActorBase* creator = nullptr;
            // if this is engine::module::IModule::getRuntimeTypeInfoStatic()::sTypeInfo, then it becomes an imaginary autobuilder actor
            void* creator_rtti = nullptr;
            sead::Heap* instance_heap = nullptr;
            sead::TypedBitFlag<TransformFlags> transform_flags;
        };
        static_assert(sizeof(CreateArg) == 0x98);

        enum Result : u32 {
            FailedToAcquireWatcher = 0,
            _01 = 1,
            EmptyActorName = 2,
            FailedToCreatePreActor = 3,
            FailedToCreateBlackboard = 4,
            EmptyNameRef = 5,
            NoMatchingFactory = 6,
            FailedToAllocateRequest = 7,
            InvalidWatcher = 8,
        };

        /*
        beginning of actual class is something like
        PresenceJudge* mActorPresenceJudge;
        sead::ObjArray<ActorFactory> mActorFactories;
        ...
        */
    };

    class PreActor {
        public:
        u64 field0_0x0;
        u32 mActorHash;
        u32 mPresenceFlagHash;
        float mExtraCreateRadius;
        u32 mPresenceFlags; // | 1 is DeleteAfterClearedSageOfFire, | 8 is spawn, | 0x10 is drop, | 0x20 is IsTreasureBoxOpen, | 0x40 is IsTreasureBoxUnlock. | 0x80 is IsAppearKorok
        void* pSettings; //struct ActorSettings *mpSettings;
#ifdef TOTK_100
        u64 _idkman[8]; // XXX 0x40 extra somewhere before mActor, no idea where it goes or if it throws off later records
#endif
        engine::actor::ActorBase* mActor;
        void* _7; // struct ActorParamMutexInfo *field7_0x28;
        u64 field8_0x30;
        u64 field9_0x38;
        sead::Vector3f mPosition;
        sead::Matrix33f mRotation;
        sead::Vector3f mScale;
    };

} // namespace actor

