//#include <gfx/seadCamera.h>
#include "util/camera.hpp"
#include "ActorWatcher.hpp"
#include "TextWriter.hpp"

namespace lotuskit::util::camera {

    void freezeAtActorWatcher(u64 slot_i, float dist, const sead::Vector3f &dir, float elev, bool follow, bool relative) {
        followSlot = slot_i;
        followDist = dist;
        followDir = dir;
        followElev = elev;
        doFollowActor = follow; // continuously
        doFollowActorRelativeRotate = relative;
        followActorImpl(); // once
        doFreeze = true;
    }

    void followActorImpl() {
        const auto* actor = ActorWatcher::getSlotActor(followSlot);
        if (actor == nullptr) {
            TextWriter::toastf(5, "[error] camera::freezeAtActorWatcher(slot_i=%p) null\n", followSlot);
            doFollowActor = false;
            return;
        }

        sead::Vector3f targetAt = actor->mPosition;
        frozenAt = {targetAt.x, targetAt.y, targetAt.z}; // cast to double
        frozenUp = {0, 1, 0};
        // assert followDir is a unit vec3f

        sead::Vector3f cameraPos;
        sead::Vector3f dir = doFollowActorRelativeRotate ? actor->mRotation * followDir : followDir;
        cameraPos = targetAt - dir * followDist + sead::Vector3f{0,1,0} * followElev;
        frozenPos = {cameraPos.x, cameraPos.y, cameraPos.z}; // cast to double
    }

} // ns

