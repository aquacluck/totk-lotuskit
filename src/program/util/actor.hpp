#pragma once
#include "lib.hpp"
#include <string>

#include "structs/engineActor.hpp"
using ActorBase = engine::actor::ActorBase;

namespace lotuskit::util::actor {
    void createSimple(const std::string &actorName);
    void createSimpleXYZ(const std::string &actorName, float x, float y, float z);
    void createAndWatch(u8 slotIndex, const std::string &actorName);
    void createAndWatchXYZ(u8 slotIndex, const std::string &actorName, float x, float y, float z);

    // FIXME too many args, but AS arrays aren't wired to sead containers yet
    void setPosXYZ(ActorBase* actor, float x, float y, float z);
    void setRot9(ActorBase* actor, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
    void setPosRot39(ActorBase* actor, float x, float y, float z, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
    void setPosRot(ActorBase* actor, sead::Vector3f, sead::Matrix33f);

} // ns
