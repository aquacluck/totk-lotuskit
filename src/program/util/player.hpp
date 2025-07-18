#pragma once
#include <math/seadVector.h>

namespace game::component { class PlayerComponent; }

namespace lotuskit::util::player {
    void InstallHooks();
    game::component::PlayerComponent* getPlayerComponent();

    inline u8 doLStickAbsoluteMode = 0; // 0: vanilla, 1: radOffset, 2: targetPos, 3: targetActorWatcher, 4: freezeAtCurrentCamera, 5: Player
    inline float doLStickAbsoluteRadOffset = 0;
    inline sead::Vector3f doLStickAbsoluteTargetPos = {0, 0, 0};
    inline size_t doLStickAbsoluteTargetActorWatcher = 0; // slot_i
    inline void doLStickAbsoluteVanilla() { doLStickAbsoluteMode = 0; }
    inline void doLStickAbsoluteRadOffset_set(float v) { doLStickAbsoluteMode = 1; doLStickAbsoluteRadOffset = v; }
    inline void doLStickAbsoluteTargetPos_set(const sead::Vector3f& v) { doLStickAbsoluteMode = 2; doLStickAbsoluteTargetPos = v; }
    inline void doLStickAbsoluteTargetActorWatcher_set(size_t v) { doLStickAbsoluteMode = 3; doLStickAbsoluteTargetActorWatcher = v; }

    inline float doLStickAbsoluteCameraFreezeXDump = 0; // these are written constantly, then copied to source for camera freeze
    inline float doLStickAbsoluteCameraFreezeZDump = 0;
    inline float doLStickAbsoluteCameraFreezeX = 0; // these are the source for camera freeze
    inline float doLStickAbsoluteCameraFreezeZ = 0;
    inline void doLStickAbsoluteCameraFreeze() {
        doLStickAbsoluteMode = 4; // FIXME source xz from util::camera if doFreeze
        doLStickAbsoluteCameraFreezeX = doLStickAbsoluteCameraFreezeXDump;
        doLStickAbsoluteCameraFreezeZ = doLStickAbsoluteCameraFreezeZDump;
    }
    inline void doLStickAbsolutePlayer() { doLStickAbsoluteMode = 5; }

    //void disableGloom(bool);
    //void setLinearVelocity(sead::Vector3f* vel);
    //void setLinearVelocityXYZ(float x, float y, float z);

    float getStamina();
    void setStamina(float amount);
    inline float freezeStaminaRecoveryAmount = 0; // 0: vanilla, <0: never recover, >0: always recover amount (bypass exhaustion + some other recovery blockers)
    inline void freezeStaminaRecovery(float amount) { freezeStaminaRecoveryAmount = amount; }
    inline bool freezeStaminaExhaustionMode = false;
    inline void freezeStaminaExhaustion(bool exhausted) { freezeStaminaExhaustionMode = exhausted; }
    inline bool doTextWriterStaminaCalculator = false;
    inline void doTextWriterStaminaCalculator_set(bool v) { doTextWriterStaminaCalculator = v; }

} // ns

