#pragma once

#include "structs/bbBlackboard.hpp"
#include "structs/engineActor.hpp"

namespace engine {

namespace event {

class EventMgr;

class ActorLink : public actor::ActorBaseLink {
public:
    void checkDerivedRuntimeTypeInfo(void*) const override;
    void* getRuntimeTypeInfo() const override;
};
//static_assert(sizeof(ActorLink) == 0x18); // XXX is this different from engine::actor::ActorLink?

struct EventRequestArg {
    EventRequestArg(const sead::SafeString& event) {
        // since we don't have an implementation for FixedSafeString constructors, we will manually copy them here
        auto len = strnlen(event.cstr(), 128);
        strncpy(event_name.getBuffer(), event.cstr(), len + 1 > 128 ? 128 : len + 1);
        *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(&event_name) + 0x8) = len + 1 > 128 ? 128 : len + 1;
    }

    bb::InitInfo<32> blackboard_info;
    sead::Matrix34f _1918;
    int start_frame = -1;
    u16 _194c;
    u8 _194e;
    u8 _194f;
    ActorLink event_starter_link;
    ActorLink _1968;
    sead::FixedSafeString<256> _1980;
    sead::FixedSafeString<128> event_name;
};
static_assert(sizeof(EventRequestArg) == 0x1b20);

} // namespace event

} // namespace engine


/*
typedef enum EventRequestState {
    WaitRequest=0,
    Requested=1,
    SomeoneReject=2,
    Success=3,
    EventFinish=4
} EventRequestState;

namespace engine::component {
class EventMemberComponent {
    public:

    engine::component::IActorComponent mBase;
    engine::actor::ActorBase *mpActor;
    undefined8 field2_0x18;
    ushort mFlags; // 2 = AlwaysEventMember, 3 = DisableHideNonEventMember, 4 = NoPauseASOnNotParticipate
    undefined field4_0x22;
    undefined field5_0x23;
    enum EventRequestState mEventRequestState;
    pp::Document *mpEventMemberParamDocument;
    sead::Container::OffsetList mCallbackList;
};
} // ns


namespace engine::component {
class EventPerformerComponent {
    public:

    engine::component::EventMemberComponent mBase;
    ai::AIControllerActor *mpAIController;
    undefined8 field2_0x50;
    ai::AIControllerSet *mAIControllerSet;
    undefined8 field4_0x60;
    undefined8 field5_0x68;
    engine::event::EventArg *field6_0x70;
    undefined8 field7_0x78;
    int field8_0x80;
    ushort field9_0x84;
    enum AIType mDriveAIType;
    undefined field11_0x87;
};
} // ns


namespace engine::event {
class EventRequestInfo {
    public:

    engine::bb::BBUpdateInfo<32> mArguments;
    Matrix34f field1_0x1918;
    int mStartFrame;
    ushort field3_0x194c;
    byte field4_0x194e;
    byte field5_0x194f;
    engine::actor::BaseProcLink mStarterLink;
    engine::actor::BaseProcLink field7_0x1968;
    sead::Prim::SafeString::FixedSafeString<256> field8_0x1980;
    sead::Prim::SafeString::FixedSafeString<128> mName;
};
} // ns


namespace engine::event {
class EventNode {
    public:

    undefined *mVtable;
    sead::Prim::SafeString::FixedSafeString<128> mName;
    engine::actor::BaseProcLink mStarterLink;
    enum EventPriority mEventPriority;
    bool mIsValidActor;
    bool mIsSubEvent;
    bool mIsNotRejectByMainEvent;
    bool field7_0xb7;
    Matrix34f field8_0xb8;
    int mStartFrame;
    byte field10_0xec;
    byte field11_0xed;
    byte field12_0xee;
    byte field13_0xef;
    engine::actor::BaseProcLink field14_0xf0;
    sead::Prim::SafeString::FixedSafeString<256> field15_0x108;
    engine::event::BBUpdateInfo<16> mArguments;
    engine::event::PtrArray<EventControllerBase> mControllerArray;
};
} // ns

namespace engine::event {
class EventActor {
    public:

    int mCount;
    int field1_0x4;
    engine::actor::CreatorActorLink *mpActorCreateLink;
    SafeString mName;
};
} // ns

namespace engine::event {
class EventMgr {
    public:

    undefined *mVtable;
    float mDeltaFrame;
    float mDeltaTime;
    float field3_0x10;
    float field4_0x14;
    undefined4 field5_0x18;
    int field6_0x1c;
    int mEventCount;
    uint mPauseHashes[3];
    undefined4 field9_0x30;
    uint field10_0x34;
    uint field11_0x38;
    uint field12_0x3c;
    sead::Heap::ExpHeap *mpEventHeap;
    undefined8 field14_0x48;
    undefined8 field15_0x50;
    engine::event::Event *mpCurrentActiveEvent;
    engine::event::EventMgrCallback *mpEventMgrCallback;
    engine::event::EventArg *field18_0x68;
    undefined8 field19_0x70;
    engine::event::EventMemberMgr *field20_0x78;
    undefined8 field21_0x80;
    sead::Thread::CriticalSection field22_0x88;
    uint mCalcFlags;
    uint field24_0xac;
    sead::Prim::SafeString::FixedSafeString<128> mCurrentEventName;
    sead::Prim::SafeString::FixedSafeString<256> mResidentEventListPath;
    engine::event::Buffer<ResidentEvent> mGlobalResidentEventArray;
    engine::event::PtrArray<Event> mActiveEventArray;
    void field29_0x270[16];
    engine::event::ObjList<EventNode> mEventNodeList;
    engine::event::EventNode mEventNodeBuffer[128];
    undefined4 field32_0x7631c;
    sead::Thread::DelegateThread *mEventSettingLoadThread;
    sead::Prim::DelegateBase mEventSettingLoadDelegate;
    sead::Thread::DelegateThread *mResidentEventLoadThread;
    sead::Prim::DelegateBase mResidentEventLoadDelegate;
    sead::Prim::IFunction field37_0x76370;
    sead::Prim::IFunction field38_0x76390;
    undefined4 mEventSettingLoadThreadAffinity;
    undefined4 field40_0x763b4;
    undefined8 field41_0x763b8;
    undefined8 field42_0x763c0;
    undefined8 field43_0x763c8;
    undefined8 field44_0x763d0;
    undefined8 field45_0x763d8;
    sead::Prim::IFunction field46_0x763e0;
    sead::Heap::IDisposer mStaticDisposer;
};
} // ns


Common_EventStarter.pack
CookSet_EventStarter.pack
DungeonBoss_Gerudo_EventStarter.pack
DungeonBoss_Goron_EventStarter.pack
DungeonBoss_Rito_EventStarter.pack
DungeonBoss_Zonau_EventStarter.pack
DungeonBoss_Zora_EventStarter.pack
Enemy_EventStarter.pack
Enemy_PhantomGanon_EventStarter.pack
HiddenKorok_EventStarter.pack
KorokCarry_EventStarter.pack
Npc_EventStarter.pack
RecycleBox_EventStarter.pack
Sage_EventStarter.pack
SkyObj_ZonauRobot_Background_EventStarter.pack
TreasureBox_EventStarter.pack


*/
