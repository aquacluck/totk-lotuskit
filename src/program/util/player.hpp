#pragma once
#include <math/seadVector.h>

namespace game::component { class PlayerComponent; }

namespace lotuskit::util::player {
    void InstallHooks();
    game::component::PlayerComponent* getPlayerComponent();

    // XXX LStick helpers are exposed to script in tas ns, but the hooks are more appropriate in here, idk
    inline u8 doLStickAbsoluteMode = 0; // 0: vanilla, 1: radOffset, 2: targetPos, 3: targetActorWatcher
    inline float doLStickAbsoluteRadOffset = 0;
    inline sead::Vector3f doLStickAbsoluteTargetPos = {0, 0, 0};
    inline size_t doLStickAbsoluteTargetActorWatcher = 0; // slot_i
    inline void doLStickAbsoluteVanilla() { doLStickAbsoluteMode = 0; }
    inline void doLStickAbsoluteRadOffset_set(float v) { doLStickAbsoluteMode = 1; doLStickAbsoluteRadOffset = v; }
    inline void doLStickAbsoluteTargetPos_set(const sead::Vector3f& v) { doLStickAbsoluteMode = 2; doLStickAbsoluteTargetPos = v; }
    inline void doLStickAbsoluteTargetActorWatcher_set(size_t v) { doLStickAbsoluteMode = 3; doLStickAbsoluteTargetActorWatcher = v; }

    //void disableGloom(bool);
    //void setLinearVelocity(sead::Vector3f* vel);
    //void setLinearVelocityXYZ(float x, float y, float z);

} // ns

