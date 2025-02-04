#pragma once
#include <math/seadVector.h>

namespace game::component { class PlayerComponent; }

namespace lotuskit::util::player {
    void InstallHooks();
    game::component::PlayerComponent* getPlayerComponent();

    //void disableGloom(bool);
    //void setLinearVelocity(sead::Vector3f* vel);
    //void setLinearVelocityXYZ(float x, float y, float z);

} // ns

