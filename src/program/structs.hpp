#pragma once
#include "types.h"

// Thanks to Watertoon for documenting many types

struct Vector3d {
    double X;
    double Y;
    double Z;
};
struct Vector2f {
    float X;
    float Y;
};
struct Vector3f {
    float X;
    float Y;
    float Z;
};
struct Vector4f {
    float X;
    float Y;
    float Z;
    float W;
};

struct Matrix33f {
    float m11;
    float m12;
    float m13;
    float m21;
    float m22;
    float m23;
    float m31;
    float m32;
    float m33;
};

struct Matrix34f {
    float m11;
    float m12;
    float m13;
    float m14;
    float m21;
    float m22;
    float m23;
    float m24;
    float m31;
    float m32;
    float m33;
    float m34;
};

struct Quaternionf {
    float A;
    float B;
    float C;
    float D;
};

struct hkFloat3Pair {
    float m_secX;
    float m_secY;
    float m_secZ;
    float m_firstZ;
    float m_firstX;
    float m_firstY;
};

/*
 * WorldMgr {
    struct WorldMgrBase mBase;
    struct hknpWorld * mpHknpWorld; // does this relate to hknpWorldCinfo?
    //phive::WorldMgrHavok::initialize -- allocs+passes ptr to hknpWorld::hknpWorld() -- arg 2 CONCAT44(0x40a00000, *(WorldMgrBase + 0x84)) ? -- also passed to hknpWorldCinfo::hknpWorldCinfo(&local_1d0) which writes back into it? idk
    //phive::CreateWorldMgrHavok,  phive::DynamicsFramework::createWorld,  phive::CreateDynamicsFramework, PhiveBackEnd::sInstance
  }
  struct DynamicsFramework {
    struct DynamicsFrameworkBase mBase;
    struct WorldMgr * mpWorldMgr0;
    struct WorldMgr * mpWorldMgr1; // 2??
  }
  struct PhiveBackEnd {
      struct DynamicsFramework * mpDynamicsFramework
  }

*/

struct hknpMotion {
    // sequential pool of these 0x80 records. area before is mostly 0s with some pointers into the first record + other stuff. prob need to find an index into this pool?

    Quaternionf m_orientation;
    Vector3d m_centerOfMass;
    hkFloat3Pair m_previousStepLinearAndAngularVelocity;

    f16 m_inverseInertia[4];
    u32 m_firstAttachedBodyIndex; // serial_and_index? 6 1 7fffffff for player. cycles through other low values for eg UH hold. hknpBody has m_index, m_nextAttachedBodyIndex, m_id. hknpConstraint has hknpBodyIdAndIndex. hknpGravityGun hknpFirstPersonCharacterCinfo hknpCharacterProxy hknpMotionType
    u32 m_solverId;
    u8 m_padding[6];

    f16 m_integrationFactor;
    u16 m_motionPropertiesId;
    f16 m_lookAheadDistance;
    f16 m_maxRotationPerStep;

    u8 m_cellIndex;
    u8 m_spaceSplitterWeight;

    Vector4f m_linearVelocityAndSpeedLimit;
    Vector4f m_angularVelocityLocalAndSpeedLimit;

    // misc interesting hk types:
    // hkTransformd
    // hclCollidable
    // hknpCharacterProxy
    // hknpRagdoll
    // hknpMotionProperties m_flags collision flags
    // hknpWorldSnapshot: bodies, motions, constraints, ...
    // hknpBody+hknpBodyCinfo has u32 m_motionId?
};

struct TreeMapNode {
    undefined * * mVtable;
    undefined * mLeft;
    undefined * mRight;
    undefined * mParent;
    u32 mHash;
    u32 mColor;
    char * mpStringKey;
    void * mpResourceUnit;
    void * mpResourceMgr;
};

struct TransceiverBase {
    undefined * * mVtable;
    u32 mTransceiverIndex;
    undefined field2_0xc;
    undefined field3_0xd;
    undefined field4_0xe;
    undefined field5_0xf;
    struct TreeMapNode field6_0x10; // rbtree node?
};

struct MutexType {
    u8 idk[0x20];
};

struct CriticalSection { // PlaceHolder Class Structure
    struct MutexType mMutex;
};

template <class T>
class Buffer {
    public:
    u32 mCount;
    u8 field1_0x4;
    u8 field2_0x5;
    u8 field3_0x6;
    u8 field4_0x7;
    T * mppArray;
};


struct IActorComponent { // PlaceHolder Class Structure
    void * vtable;
    char * field1_0x8;
};

// GameActor inherits from ActorBase which inherits from IActor
// ActorBase is from the engine, GameActor is totk's extension of it
struct IActor {
    void * vtable;
    void * heap;
    s32 id;
    s32 pad;
    char * name;
    u8 morepad[0x1a0]; // I cannot be bothered to fill this all in lol
    u16 * array; // ring of actor relations, stores an index into ActorMgr's actor relation array
    u32 max;
    u32 offset;
    u32 count;
    // 29 on 1.0.0, 33 on 1.1.0+
    // fun fact, once you hit the limit, that's when things stop equipping when zuggle overloaded
#ifdef TOTK_100
    u16 relations[29];
#else
    u16 relations[33];
#endif
};

struct ByamlIter {
    void* mpResByaml; // ResByaml*
    u32 mContainerOffset;
    u8 mType; // ResByamlNodeType
    u8 _pad[3];
};

struct ActorInfoLink {
    undefined8 _1;
    undefined8 _2;
    undefined4 _3;
    int _4;
    Vector3f mPosition;
    Matrix33f mRotationMatrix;
    Vector3f mScale;
    undefined4 _8;
    void* mpActorSettings; // ActorSettings* (ActorObserver?)
    undefined8 _11;
    void* mpActorParamMutexInfo; // ActorParamMutexInfo*
    undefined8 _14;
    undefined8 _15;
    void* mpActorLogicBinder;
    u64 mHash;
    u32 mSrtHash;
    u32 _20;
    u32 mActorFlags;
    undefined4 _22;
    ByamlIter mByamlIterDynamic; // 0x10
    ByamlIter mByamlIterPhive; // 0x10
    undefined8 _25;
    undefined8 _26;
    undefined8 _27;
    undefined8 _28;
    u32 mPreActorIndex0;
    u32 mPreActorIndex1;
    char* mpName; // SafeString
    u32 mBucketIndex;
    undefined4 _34;
    undefined4 _35;
    u16 mPhiveGroupIndex;
    u16 _37;
    void* _38;
    void* _40;
};

struct ActorBase { // PlaceHolder Class Structure
    struct IActor mIActor;
    undefined field1_0x20e;
    undefined field2_0x20f;
    char * mpActorPackName;
    struct Buffer<IActorComponent*> mComponentArray;
    struct Buffer<u8> mComponentIndexArray;
    struct Buffer<u8> field6_0x238;
    u8 field7_0x248;
    undefined field8_0x249;
    undefined field9_0x24a;
    undefined field10_0x24b;
    undefined field11_0x24c;
    undefined field12_0x24d;
    undefined field13_0x24e;
    undefined field14_0x24f;
    void * mpActorJobFirstCalc; // ActorJobContainer*
    void * mpActorJobSecondCalc;
    void * mpActorJobThirdCalc;
    void * mpActorJobFourthCalc;
    //undefined8 mpActorJobUnknown; // XXX i was 8B off, is this the extra?
    float field20_0x278;
    struct Vector3f mHomePosition;
    struct Matrix33f mHomeRotation;
    struct Vector3f mPosition;
    struct Matrix33f mRotationMatrix; // column: Right, Up, FoV (forward?), rotation matrix?
    struct Vector3f mScale;
    struct Vector3f mUpdatePosition;
    struct Matrix33f mUpdateRotation;
    struct Matrix34f field28_0x318;
    struct Vector3f mAABBMin;
    struct Vector3f mAABBMax;
    void * mpDocumentActorParam;
    ActorInfoLink * mpActorInfoLink;
    struct TransceiverBase mTransceiverBase;
    undefined field34_0x3b8;
    undefined field35_0x3b9;
    undefined field36_0x3ba;
    undefined field37_0x3bb;
    undefined field38_0x3bc;
    undefined field39_0x3bd;
    undefined field40_0x3be;
    undefined field41_0x3bf;
    void * mpBlackboardAccessor;
    undefined8 mpCreatorInfo;
    undefined field44_0x3d0;
    undefined field45_0x3d1;
    undefined field46_0x3d2;
    undefined field47_0x3d3;
    undefined field48_0x3d4;
    undefined field49_0x3d5;
    undefined field50_0x3d6;
    undefined field51_0x3d7;
    undefined field52_0x3d8;
    undefined field53_0x3d9;
    undefined field54_0x3da;
    undefined field55_0x3db;
    undefined field56_0x3dc;
    undefined field57_0x3dd;
    undefined field58_0x3de;
    undefined field59_0x3df;
    undefined8 field60_0x3e0;
    undefined field61_0x3e8;
    undefined field62_0x3e9;
    undefined field63_0x3ea;
    undefined field64_0x3eb;
    s32 field65_0x3ec;
    u32 field66_0x3f0;
    u8 mPauseState;
    u8 field68_0x3f5;
    u8 field69_0x3f6;
    u8 field70_0x3f7;
    undefined8 field71_0x3f8;
    undefined8 mpRingHead; // 8  ring allocator of 0x20 elements
    undefined8 mpRingStart;
    s32 field74_0x410;
    s32 mRingMax;
    struct CriticalSection field76_0x418;
    u32 mBancFlags; // bit 16 = is bcett context set
    u32 field78_0x43c;
    u64 field79_0x440;
    undefined field80_0x448;
    undefined field81_0x449;
    undefined field82_0x44a;
    undefined field83_0x44b;
    undefined field84_0x44c;
    undefined field85_0x44d;
    undefined field86_0x44e;
    undefined field87_0x44f;
    struct CriticalSection field88_0x450;
    // truncated
    // struct ActorTls mActorTls; u8 field90_0x4a8; u8 field91_0x4a9; undefined field92_0x4aa; undefined field93_0x4ab; undefined field94_0x4ac; undefined field95_0x4ad; undefined field96_0x4ae; undefined field97_0x4af;
};

struct ActorRelation {
    ActorBase * parent;
    ActorBase * child;
};

struct ActorMgr {
    u8 padding[0x2f0];
    u32 count;
    u32 pad;
    ActorRelation * array;
};

struct ActorSettings {
    void* mpGameActorInfoRow; // struct GameActorInfoRow *mpGameActorInfoRow;
    void* mpLocatorDataRow; // struct LocatorDataRow *mpLocatorDataRow;
    undefined4 field2_0x10;
    char mPackActorName[64];
    char mActorNameRef[64];
    uint mClassNameCrc32;
    u32 mCalcPriority; // enum CalcPriority mCalcPriority;
    undefined4 field7_0x9c;
    char* mFmdbName; // SafeString
    char* mModelProjectName; // SafeString
    Vector3f mDebugModelScale;
    uint mInfoFlags; /* << 17 is use preactor renderer, << 20 is is far actor, << 22 is event hide actor, low bit is has miasma conversion, << 1 is use area shape type, << 2 is is respawn, << 3 is is short create radius in heavy, << 12 is is miasma swarm culling, << 4 is is ignore sys culling mgr, << 5 is use preactor area culling, << 6 is prefab merge exception, <<8 and 9 are y limit height lower and upper, << 11 is force enable area culling, << 13 is use precalculated culling on actor only, << 19 is no use preactor renderer alpha fade, << 21 is use forest renderer */
    uint mIsInitialized; /* | 2 use forest renderer, | 4 traverse for loading pause */
    undefined4 field13_0xc4;
    void* mpActorInfoRow; // struct ActorInfoRow *mpActorInfoRow;
    uint mTagTableIndex;
    undefined4 field16_0xd4;
};

struct PreActor {
    undefined8 field0_0x0;
    uint mActorHash;
    uint mPresenceFlagHash;
    float mExtraCreateRadius;
    uint mPresenceFlags; // | 1 is DeleteAfterClearedSageOfFire, | 8 is spawn, | 0x10 is drop, | 0x20 is IsTreasureBoxOpen, | 0x40 is IsTreasureBoxUnlock. | 0x80 is IsAppearKorok
    struct ActorSettings *mpSettings;
    struct ActorBase *mpActor;
    void* _7; // struct ActorParamMutexInfo *field7_0x28;
    undefined8 field8_0x30;
    undefined8 field9_0x38;
    Vector3f mPosition;
    Matrix33f mRotation;
    Vector3f mScale;
    undefined field13_0x7c;
    undefined field14_0x7d;
    undefined field15_0x7e;
    undefined field16_0x7f;
    void* mpActorLogicBinder; // struct ActorLogicBinder *mpActorLogicBinder;
    u64 mHashId;
    undefined4 field19_0x90;
    u16 mFlags;
    undefined field21_0x96;
    undefined field22_0x97;
    undefined4 field23_0x98; // | 2 is IsNegation (presence flag)
    u16 mDynamicFlags; // 0x1000 is IsCopyPresenceAtTakeOverPreActor
    undefined field25_0x9e;
    undefined field26_0x9f;
    undefined4 field27_0xa0;
    int mRefCount;
    undefined8 field29_0xa8;
    undefined8 field30_0xb0;
    undefined8 field31_0xb8;
    undefined8 field32_0xc0;
    undefined8 field33_0xc8;
    void* mBBInitInfo; // struct BBInitInfo *mBBInitInfo;
    undefined8 field35_0xd8;
    undefined8 field36_0xe0;
    undefined8 field37_0xe8;
    undefined8 field38_0xf0;
    undefined *field39_0xf8;
    void* mpPreActorListNode; // struct ListNode<PreActor*> *mpPreActorListNode;
    void* mpPreActorList; // struct ObjList<PreActor> *mpPreActorList;
};


struct ListImpl {
    void * mpListBack;
    void * mpListFront;
    s32 mCount;
    s32 mNodeOffset;
};

struct Heap { // PlaceHolder Class Structure
    void * mVtable;
    struct Heap * mpContainedHeap;
    void * mpHeapListNodePrev;
    void * mpHeapListNodeNext;
    char * mpName;
    void * mpStart;
    u64 mSize;
    struct Heap * mpParentHeap;
    struct ListImpl mChildList; // sead::Heap
    void * mpParentListNodePrev;
    void * mpParentListNodeNext;
    void * mpDisposerListBack; // sead::IDisposer
    void * mpDisposerListFront;
    s32 mDisposerListCount;
    s32 mDisposerListOffset; // 0x10
    s32 mHeapDirection; // -1 = tail, 1 = head
    u8 field16_0x84;
    u8 field17_0x85;
    u8 field18_0x86;
    u8 field19_0x87;
    struct MutexType mHeapCS;
    u16 mIsThreadSafe;
    u16 mHeapCheckTag;
};

struct IDisposer { // PlaceHolder Class Structure
    void * * mVtable;
    struct Heap * mpContainedHeap;
    void * mpHeapListNodePrev;
    void * mpHeapListNodeNext;
};

struct Job {
    void * * mVtable;
    char * mpName;
};

struct IModule {
    struct Job mJob;
    u8 field1_0x10[8];
    u32 field2_0x18;
};

struct SceneFactory {
    void * mpfSceneCreate;
    u32 mNameHash;
    u8 field2_0xc;
    u8 field3_0xd;
    u8 field4_0xe;
    u8 field5_0xf;
};

template <class T>
class PtrArrayAllocator {
    public:
    s32 mCount;
    s32 mMax;
    T * * mppArray;
    void * mpFreeList;
    void * mpMemory;
};

struct SceneOnThreadUnit {
    u8 idk[0xc8];
    //struct OnThreadUnit mOnThreadUnit;
    //struct IDelegate mDelegateExeCallback;
    //struct IFunction mIFunctionUser;
};

template <class T>
class BufferedSafeStringBase {
    public:
    T * mpString;
    int mSize;
};

template <size_t N>
class FixedSafeStringBase {
    public:
    BufferedSafeStringBase<char> mBufferString;
    char mStringStorage[N];
    u8 field2_0x8c;
    u8 field3_0x8d;
    u8 field4_0x8e;
    u8 field5_0x8f;
};

struct ISceneComponent { // PlaceHolder Class Structure
    void * * mVtable;
};

struct SceneComponentContainer {
    struct Buffer<ISceneComponent*> mSceneComponentArray;
    struct Buffer<u8> mIsSceneComponentPresentArray;
};

struct IScene {
    u8 idk[0x50];
    struct SceneComponentContainer mComponentContainer;
    // further members excluded
};

struct SceneModule { // PlaceHolder Class Structure
    struct IModule field0_0x0;
    u8 field1_0x1c[4];
    struct PtrArrayAllocator<SceneFactory> mSceneFactory;
    void * mpfCreateDefaultScene;
    struct SceneOnThreadUnit field7_0x48;
    struct SceneOnThreadUnit field8_0x110;
    struct Heap * mpHeapSceneParent; // Exp or Frame
    struct Heap * mpSceneHeap;
    struct IScene * mpCurrentScene; // structure has scene components?
    struct IScene * mpLastScene;
    struct FixedSafeStringBase<256> mNextScene;
    struct FixedSafeStringBase<256> mCurrentScene;
    u8 field15_0x418;
    u8 field16_0x419;
    u8 field17_0x41a;
    u8 field18_0x41b;
    u8 field19_0x41c;
    u8 field20_0x41d;
    u8 field21_0x41e;
    u8 field22_0x41f;

    u8 idk[57];
    //struct Document * mpDocumentSceneParam;
    //struct SceneLowPriorityThread * mpThreadSceneLowPriority;
    //struct ResourceBinder mResourceBinderSceneParam;

    u8 mIsFinalizeResourceBinder;
    u8 field27_0x459;
    u8 field28_0x45a;
    u8 field29_0x45b;
    u8 field30_0x45c;
    u8 field31_0x45d;
    u8 field32_0x45e;
    u8 field33_0x45f;
    u8 mpNextSceneArg[8];
    u8 mSceneTriggerFlags; // bit 3 = Requires scene change
    u8 field36_0x469;
    u8 field37_0x46a;
    u8 field38_0x46b;
    u8 field39_0x46c;
    u8 field40_0x46d;
    u8 field41_0x46e;
    u8 field42_0x46f;
    struct IDisposer field43_0x470;
};

typedef enum SceneComponentType {
    SequenceRef=0,
    BlackboardRef=1,
    DragonMgrParam=2,
    PlayingInfoMgr=3,
    GlobalGameParameterTable=4,
    TeraRigidBodyMgr=5,
    BancLoadMgr=6,
    LocationMgr=7,
    NavMeshMgr=8,
    StaticCompoundBodyMgr=9,
    AutoSaveMgr=10,
    LoadProgressMgr=11,
    ModelViewer=12,
    ResidentActorMgr=13,
    BakeLoadMgr=14,
    AutoPlacementMgr=15,
    TrolleyRailMgr=16,
    ScopeMgr=17,
    CompanionMgr=18,
    SensorMgr=19,
    LargeDungeonMapMgr=20,
    OwnedHorseMgr=21,
    ChallengeFollowerMgr=22,
    JunkBoxMgr=23,
    TimerMgr=24,
    LavaSolidMgr=25,
    HorablinMgr=26,
    FlightMoveMgr=27,
    RupeeRabbitMgr=28,
    LoadingPauseMgr=29,
    AmiiboMgr=30,
    EnergyGaugeMgr=31,
    WarpMgr=32,
    ActorRenderingProxy=33,
    SystemCullingMgr=34,
    RelocateObstacleMgr=35,
    LightBallBudMgr=36,
    CaveMarkerMgr=37,
    FootprintMgr=38,
    HiddenKorokMgr=39,
    ColorVariationChangeMgr=40,
    EnemyGameSpecialInfoMgr=41,
    DropMgr=42,
    LimiterActorCountMgr=43,
    BattleTipsGolemMgr=44,
    CompoundNavMeshObjectMgr=45,
    DestructibleMgr=46,
    GameSafetyMgr=47,
    LowGravityMgr=48,
    ShooterMgr=49,
    AreaCullingMgr=50,
    RouteRailMgr=51,
    CraftSignboardMgr=52,
    WildHorseMgr=53
} SceneComponentType;

struct ActorNode {
    u8 idk[0x40];
    ActorBase* mpActor; // XXX IActor/ActorBase/etc???
};

struct ActorLink {
    u8 idk[8];
    ActorNode * mpActorNode;
};

struct Counter {
    float mDeltaFrames;
    float mDeltaTime;
    int * mTimerMultiplierIndices;
    int mMaxTimerMultiplierCount;
    int mTimeMultiplierBaseIndex;
    int mTimeMultiplierIndex;
    undefined4 field6_0x1c;
    undefined8 field7_0x20;
};

struct TimeSpeedMultiplier {
    float mValue;
    float mTarget;
};

struct VFRMgr {
    float mRawDeltaFrame;
    float mRawDeltaTime;
    float mDeltaFrame;
    float mDeltaTime;
    struct Counter mPerCoreCounters[3];
    struct TimeSpeedMultiplier mTimeSpeedMultipliers[16];
    float mIntervalValue;
    uint mCoreAndIntervalMask;
    struct IDisposer mStaticDisposer;
};

