#pragma once
#include <container/seadPtrArray.h>
// thanks dt: https://github.com/dt-12345/ZuggleAI/blob/master/source/program/misc.h

namespace PracticeMod {
    // bunch of misc structs I don't feel like organizing

    struct EquipmentUserComponent;
    struct DynamicEquipmentComponent;

    // this is for 1.1.0+ but whatever
    struct GameActor {
        char _[0x218];
        char* name;
        sead::PtrArray<void*> components;

        EquipmentUserComponent* get_equipment_user_component() {
            return reinterpret_cast<EquipmentUserComponent*>(components.at(70));
        }
        DynamicEquipmentComponent* get_dynamic_equipment_component() {
            return reinterpret_cast<DynamicEquipmentComponent*>(components.at(65));
        }
    };

    struct LinkData {
        char _00[0x40];
        GameActor* actor;
        int actor_id;
        u64 refs;
        u8 _058;
    };

    struct ActorLink {
        void* vtable;
        LinkData* link_data;
        int actor_id;
        u8 stuff[4];

        bool is_valid() const {
            if (!link_data || !(link_data->actor)) return false;
            return true;
        }

        GameActor* get_actor() {
            if (is_valid()) return link_data->actor;
            return nullptr;
        }
    };
    static_assert(sizeof(ActorLink) == 0x18);

    struct SlotState {
        u32 state;
        u32 equip_state;
    };

    struct EquipmentUserComponent {
        char _00[0x18];
        GameActor* actor;
        ActorLink dynamic_equipment[8];
        ActorLink static_equipment[12];
        ActorLink _200;
        char _218[0x439c - 0x218];
        SlotState slots[20];
        char _443c[0x4588 - 0x443c];
        ActorLink weapon;
        ActorLink shield;
        ActorLink bow;
        char _45d0[0x4750 - 0x45d0];
    };
    static_assert(offsetof(EquipmentUserComponent, _200) == 0x200);
    static_assert(offsetof(EquipmentUserComponent, weapon) == 0x4588);
    static_assert(sizeof(EquipmentUserComponent) == 0x4750);

    enum DynamicEquipmentState : u32 {
        Unequiped = 0,
        EquipedDrawn = 1,
        EquipedSheathed = 2,
        Thrown = 3,
    };

    struct VFRCounter {
        float time;
        float last_time;
        float rate;
    };

    // too lazy to fill this in for equipment, I have the one for shootables kinda though
    struct StateBase {
        virtual StateBase* checkDerivedRuntimeTypeInfo(void*);
        virtual void* getRuntimeTypeInfo();
        virtual ~StateBase();
        virtual void f04();
        virtual void f05();
        virtual void f06();
        virtual void f07(float*);
        virtual void f08();
        virtual void f09(float*);

        VFRCounter timer;
        u8 _14;
        GameActor* actor;
    };
    static_assert(sizeof(StateBase) == 0x20);

    struct EquipmentStateController {
        u8 flags;
        DynamicEquipmentState state;
        DynamicEquipmentState previous_state;
        StateBase* current_state;
        StateBase* states[4]; // one for each state
    };

    // this is also the base class for WeaponComponent, ShieldComponent, BowComponent, etc.
    // dynamic equipment has dynamic state (see the enum above), static equipment does not (e.g. armor)
    struct DynamicEquipmentComponent {
        char _00[0x3d0];
        EquipmentStateController state_controller;
        // cannot be bothered to fill in this
    };

    struct CoreCounter {
        float raw_delta_frame;
        float raw_delta_time;
        // time mult index ring buffer but I'm lazy
    };

    struct AttachmentComponent {
        char _00[0x18];
        GameActor* actor;
        char _20[0x420 - 0x20];
        u32 state;
    };
    static_assert(offsetof(AttachmentComponent, state) == 0x420);

    // Pouch.Weapon.EquipIndex
    inline constexpr u32 weapon_index = 0xb80e3dbb;
    // Pouch.Bow.EquipIndex
    inline constexpr u32 bow_index = 0x2cf18d2b;
    // Pouch.Shield.EquipIndex
    inline constexpr u32 shield_index = 0x350ad90e;

    struct PouchContentMgr {
        char _00[0x188];
        int current_index;
    };

    struct StructHandle {
        void* mpStruct;
        int mIndex;
        bool mIsArray;
    };

    inline u32 version = 0xffffffff;
    inline void** s_GmdMgrPtr = nullptr;
    inline void** s_VFRMgrPtr = nullptr;
    inline PouchContentMgr** s_PouchMgrPtr = nullptr;

    using Dtor = void (ActorLink*);
    inline Dtor* ActorLinkDtor = nullptr;

    using SetInt = void (void*, int, u32);
    inline SetInt* setInt = nullptr; // ideally use gmd::GameDataMgr::setIntArray and check the current pouch index in game::pouchcontent::PouchMgr

    using GetInt = bool (void*, int*, u32);
    inline GetInt* getInt = nullptr;

    using GetStructStruct = bool (void*, StructHandle&, const StructHandle&, u32);
    inline GetStructStruct* getStructStruct = nullptr;

    using SetStructEnum = void (void*, u32, const StructHandle&, u32);
    inline SetStructEnum* setStructEnum = nullptr;

    using SetStructInt = void (void*, s32, const StructHandle&, u32);
    inline SetStructInt* setStructInt = nullptr;

    using CalcHash = u32 (const char*, u32);
    inline CalcHash* calcHash = nullptr;

    using SetPendingDrop = void (DynamicEquipmentComponent**);
    inline SetPendingDrop* setPendingDrop = nullptr; // tbh, I could just implement this function myself bc it's literally setting a single value

    using GetCounter = CoreCounter* (void*);
    inline GetCounter* getCoreCounter = nullptr;

    using RequestPause = bool (GameActor*, u32);
    inline RequestPause* requestPause = nullptr;

    enum Category {
        Weapon, Bow, Arrow, Shield, Armor, Material, Food, SpecialParts, KeyItem, Rupee, Grain, SpecialPower
    };

    using EquipFunc = bool (void*, Category, int, int);
    inline EquipFunc* equipItem = nullptr;

    struct ByamlIter {
        void* mpResByaml;
        u32 mContainerOffset;
        u8 mNodeType;
    };

    struct PouchActorInfoRow {
        char* mpRowId;
        char* mpArmorEffectType;
        char* mpArmorHeadSwapActor;
        char* mpArmorNextRankActor;
        char* mpBowQuiverName;
        char* mpBundleActor;
        char* mpBundleActorNumGameData;
        char* mpCapsuleContent;
        char* mpColorVariationType;
        char* mpConversionTargetActor;
        char* mpCureEffectType;
        char* mpDraftContent;
        char* mpFakeActorName;
        ByamlIter mHandleAsPartArmorListIter;
        char* mpParasailPattern;
        char* mpPouchCategory;
        char* mpPouchGetType;
        char* mpPouchSpecialDeal;
        char* mpPouchUseType;
        char* mpPrimaryFoodMaterial;
        ByamlIter mReadyMadeCookRawMaterialsIter;
        char* mpTreasureMapContent;
        char* mpWeaponReplaceSheathName;
        char* mpWeaponReplaceSheathNameBindZ;
        char* mpWeaponSheathName;
        char* mpWeaponSubType;
        char* mpWeaponType;
        u32 mArmorHeadMantleType;
        int mArmorRank;
        u32 mArmorRequestMantlePattern;
        u32 mArmorSelfMantlePattern;
        int mAttachAddAttackValueWithZonau;
        float mAttachMulAttackValue;
        float mAttachMulGuardValue;
        int mBowShotNum;
        int mBundleActorNum;
        int mBuyingPrice;
        int mCureEffectiveTime;
        int mCureEffectLevel;
        u32 mDyeColor;
        float mDynamicEquipmentPopRot[3];
        int mEnergyRecover;
        int mEquipmentPerformance;
        int mHitPointRecover;
        int mMaxRecordExtraLife;
        int mNoUseEnergyTime;
        int mPouchSortKey;
        int mSellingPrice;
        int mSpiceBoostEffectiveTime;
        int mSpiceBoostHitPointRecover;
        int mSpiceBoostMaxHeartLevel;
        int mSpiceBoostStaminaLevel;
        int mSpiceBoostSuccessRate;
        bool mBundleIsUseUniqueName;
        bool mCannotSell;
        bool mIsCustomHorseTack;
        bool mIsPlantableCrop;
        bool mIsUsable;
        bool mPouchStockable;
    };
    static_assert(sizeof(PouchActorInfoRow) == 0x160);

} // ns

