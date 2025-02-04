#pragma once

#include "structs/exkingEnums.hpp"
#include "structs/bbBlackboard.hpp"
#include "structs/engineComponent.hpp"
#include "structs/engineTransceiver.hpp"

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

namespace engine::component { class Component; }
namespace game::component { class PlayerComponent; }
using EngineComponentIndex = engine::component::ComponentIndex;
using   GameComponentIndex =   game::component::ComponentIndex;


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

    class ActorAIGroupController {
        public:
        u64 mHash; // HACK object/actor hash
        PreActor* mpPreActorLink;
        void* idk;
    };

    class AIGroup {
        public:
        void* mVtable; // engine::actor::vft_engine_actor_AIGroup *mVtable;
        u64 mHash;
        u32 mPreActorCount;
        u8 field3_0x14;
        u8 field4_0x15;
        u8 field5_0x16;
        u8 field6_0x17;
        engine::actor::ActorAIGroupController *mpAIControllerUnitArray;
        /*
        engine::actor::ActorLogic *mpActorLogic;
        engine::actor::ActorAIGroupController *mpAIControllerUnit;
        bb::Blackboard *mpSharedBlackboard;
        engine::actor::SharedBlackboardAccessor *mpBlackboardAccessor;
        engine::actor::AIGroupTransceiver mTransceiver;
        u32 mAIGroupArrayIndex;
        u32 mAIGroupIndex;
        void* field15_0x98;
        pp::PtrArray<pp::Document> mBlackboardDocumentArray;
        */
    };

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

    class ActorTransceiver : public ::engine::transceiver::TransceiverBase {
        public:
        ~ActorTransceiver() override;
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

        engine::component::ModelComponent* getModelComponent() const {
            u32 i = static_cast<u32>(EngineComponentIndex::ModelInfoRef);
            return reinterpret_cast<engine::component::ModelComponent*>(mComponents[i]);
        }

        engine::component::ASComponent* getASComponent() const {
            u32 i = static_cast<u32>(EngineComponentIndex::ASRef);
            return reinterpret_cast<engine::component::ASComponent*>(mComponents[i]);
        }

        engine::component::PhysicsComponent* getPhysicsComponent() const {
            u32 i = static_cast<u32>(EngineComponentIndex::PhysicsRef);
            return reinterpret_cast<engine::component::PhysicsComponent*>(mComponents[i]);
        }

        game::component::PlayerComponent* getPlayerComponent() { // FIXME this belongs under GameActor?
            u32 i = static_cast<u32>(GameComponentIndex::PlayerRef);
            return reinterpret_cast<game::component::PlayerComponent*>(mComponents[i]);
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

#ifdef TOTK_100
        sead::Vector3f mPosition;
        sead::Matrix33f mRotation;
        sead::Vector3f mScale;
        u8 _align8idk[4];
#endif

        void* pSettings; //struct ActorSettings *mpSettings; // XXX guessing this goes here?
        engine::actor::ActorBase* mActor;
        void* _7; // struct ActorParamMutexInfo *field7_0x28;
        u64 field8_0x30;
        u64 field9_0x38;

#ifndef TOTK_100
        sead::Vector3f mPosition;
        sead::Matrix33f mRotation;
        sead::Vector3f mScale;
        u8 _align8idk[4];
#endif

        void* mpActorLogicBinder; // engine::actor::ActorLogicBinder *mpActorLogicBinder;
        u64 mInstanceId; // banc entity hash
        u32 field19_0x90;
        u16 mFlags;
        u8 field21_0x96; // 0 on 1.0, ff on 1.2.1?
        u8 field22_0x97; // 0/ff?
        u32 mStateFlags; // 0 = dead, 2 = spawn, 3 = culled maybe?, 6 = has linked actor, 7 = disable until untraverse, 0xd = unload?, 0x11 = traversing?, 0x16 = dynamic actor
        u16 mDynamicFlags; // 0x1000 is IsCopyPresenceAtTakeOverPreActor
        u8 field25_0x9e;
        u8 mLoadPriority;

#ifndef TOTK_100
        u32 field27_0xa0; // this 0x10 is near the end in 100
        s32 mGroupIndex;
        u32 field29_0xa8;
        s16 field30_0xac;
        s16 field31_0xae;
#endif

        void* idk[2];// byaml::ByamlIter field32_0xb0; // len 0x10
        void* field33_0xc0;
        u32 field34_0xc8;
        u8 field35_0xcc;
        u8 field36_0xcd;
        u8 field37_0xce;
        u8 field38_0xcf;

        void* mBBInitInfo; //  engine::bb::BBInitInfo *mBBInitInfo;
        engine::actor::AIGroup* mpAIGroup; // game::actor::ActorAIGroup *mpAIGroup;

        void* _idk2[4];

#ifdef TOTK_100
        u32 field27_0xa0; // XXX order might be wrong within this block + identifiers from 121 struct
        s32 mGroupIndex;
        u32 field29_0xa8;
        s16 field30_0xac;
        s16 field31_0xae;
#endif

    };

} // namespace actor

