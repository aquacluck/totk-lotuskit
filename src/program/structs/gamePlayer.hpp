#pragma once
#include "structs/engineActor.hpp"
#include "structs/engineUtil.hpp"

namespace game::player {
    class StaminaCalculator {
        public:

        virtual ~StaminaCalculator(); // mVtable
        engine::actor::ActorBase* mpPlayer; // FIXME GameActor?
        u32 idk0;
        u32 idk1;
        s32 mDeltaStaminaValue;
        s32 idk2;
        u8 idk_Receiver_StaminaMessage[28];
        u32 idk3;
        engine::util::VFRCounter mNoRecoveryTimer;
        u32 idk4;

        float mStaminaRecoverAmount;
        float mDeltaTime;
        u8 mFlags; // 0 = is tired, 1 = recovering stamina
        u8 idk5[3];
        float mStamina;
        float mExtraStamina;
        u32 mStaminaHash;
        u32 mMaxStaminaHash;
        u32 mExtraStaminaHash;
        u32 mIsTiredHash;
        float mStaminaInternal;
        float mExtraStaminaInternal;
        s32 mDeltaStaminaAdj1000;
    };
    static_assert(sizeof(StaminaCalculator) == 0x80);
    static_assert(offsetof(StaminaCalculator, mNoRecoveryTimer) == 0x40);
    static_assert(offsetof(StaminaCalculator, mStamina) == 0x5c);
}
