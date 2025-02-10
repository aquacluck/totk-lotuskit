#pragma once
#include "exlaunch.hpp"
#include <string>

#include "structs/as.hpp"
#include "structs/engineActor.hpp"
using ActorBase = engine::actor::ActorBase;
#include "structs/phive.hpp"

namespace lotuskit::util::actor {
    void createSimple(const std::string &actorName);
    void createSimpleXYZ(const std::string &actorName, float x, float y, float z);
    void createSimplePos(const std::string &actorName, const sead::Vector3f &pos);
    void createSimplePosRot(const std::string &actorName, const sead::Vector3f &pos, const sead::Matrix33f &rot);

    void createAndWatch(size_t slotIndex, const std::string &actorName);
    void createAndWatchXYZ(size_t slotIndex, const std::string &actorName, float x, float y, float z);
    void createAndWatchPos(size_t slotIndex, const std::string &actorName, const sead::Vector3f &pos);
    void createAndWatchPosRot(size_t slotIndex, const std::string &actorName, const sead::Vector3f &pos, const sead::Matrix33f &rot);

    void setPos(ActorBase* actor, const sead::Vector3f &pos);
    void setPosXYZ(ActorBase* actor, float x, float y, float z);
    void setRot(ActorBase* actor, const sead::Matrix33f &rot);
    void setRot9(ActorBase* actor, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
    void setPosRot39(ActorBase* actor, float x, float y, float z, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
    void setPosRot(ActorBase* actor, const sead::Vector3f &pos, const sead::Matrix33f &rot);

    phive::RigidBodyEntity* getMainRigidBody(ActorBase* actor);
    phive::RigidBodyEntity* getRigidBodyByName(ActorBase* actor, const std::string& name);

    // TODO extract what we can to dedicated ns: as/ai/bb/util/??? (good time to split decl/member/etc into separate methods too)
    enum class BBDumpMode: u32 { MEMBERS = 0, ENUMCLS_FILE_DECL = 1 };
    //ai::Blackboard* getAIBlackboard(ActorBase* actor);
    as::Blackboard* getASBlackboard(ActorBase* actor);
    void dumpASBlackboard(ActorBase* actor, u32 dumpMode=0);

} // ns
