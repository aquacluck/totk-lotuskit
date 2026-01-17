#pragma once
#ifdef TOTK_100
#include "misc.h"
#include <container/seadPtrArray.h>
// thanks dt: https://github.com/dt-12345/ZuggleAI/blob/master/source/program/zuggle.h
using namespace PracticeMod;
#include "engine/AI.h"

class ExecuteZuggle: public engine::ai::ExecuteBase {
    SEAD_RTTI_OVERRIDE(ExecuteZuggle, engine::ai::ExecuteBase)

    public:
    AI_NODE_NAME_CATEGORY("ExecuteZuggle", "AI");

    void parseParameters_() override;
    void enterImpl_(const ai::NodeCalcArg&) override;
    void updateImpl_(const ai::NodeCalcArg&) override;
    void leaveImpl_(const ai::NodeCalcArg&) override;

    private:
    enum State {
        Zuggle, WaitZuggle, Reequip, WaitReequip
    };

    ai::BoolInput mIsDynamic;
    ai::BoolInput mIsReequip;
    ai::BoolInput mIsShield;
    ai::BoolInput mIsWeapon;
    ai::BoolInput mIsBow;
    ai::S32Input mCount;
    int mShieldIndex = -1;
    int mWeaponIndex = -1;
    int mBowIndex = -1;
    int mNumZuggled = 0;
    State mState = Zuggle;

    EquipmentUserComponent* getEquipmentUserComponent();
};
#endif

namespace PracticeMod::zuggle {
    #ifdef TOTK_100
        void SetupStatic();
    #else
        inline void SetupStatic() { } // nop
    #endif
}

