#include "ActorWatcher.hpp"
#include "util/actor.hpp"
#include "syms_merged.hpp"
#include "structs/bbBlackboard.hpp"
#include "script/globals.hpp"
#include <prim/seadSafeString.h>

namespace lotuskit::util::actor {
    using CreateFunc = bool (engine::actor::ActorMgr*, const sead::SafeString&, const engine::actor::BaseProcMgr::CreateArg&,
                             engine::actor::CreateWatcherRef*, engine::actor::CreatePriority, engine::actor::PreActor*,
                             engine::actor::ActorFile*, sead::Function*, bool, engine::actor::BaseProcMgr::Result*, engine::actor::PreActor**);
    class ForceSetMatrixArg {
        public:
        float f[12];
        inline ForceSetMatrixArg(const sead::Vector3f& pos, const sead::Matrix33f& rot) {
            f[3] = pos.x; f[7] = pos.y; f[11] = pos.z;
            f[0] = rot.m[0][0]; f[1] = rot.m[0][1]; f[ 2] = rot.m[0][2];
            f[4] = rot.m[1][0]; f[5] = rot.m[1][1]; f[ 6] = rot.m[1][2];
            f[8] = rot.m[2][0]; f[9] = rot.m[2][1]; f[10] = rot.m[2][2];
        }
        inline void unfuck(sead::Vector3f& pos, sead::Matrix33f& rot) {
            pos.x = f[3]; pos.y = f[7]; pos.z = f[11];
            rot.m[0][0] = f[0]; rot.m[0][1] = f[1]; rot.m[0][2] = f[2];
            rot.m[1][0] = f[4]; rot.m[1][1] = f[5]; rot.m[1][2] = f[6];
            rot.m[2][0] = f[8]; rot.m[2][1] = f[9]; rot.m[2][2] = f[10];
        }
    };
    using ForceSetMatrixFunc = void (engine::actor::ActorBase*, ForceSetMatrixArg*, u32);

    void createSimple(const std::string &actorName) {
        if (lotuskit::script::globals::ResidentActors::Player == nullptr) {
            return createSimpleXYZ(actorName, 0, 0, 0);
        }
        auto pos = lotuskit::script::globals::ResidentActors::Player->mPosition;
        createSimpleXYZ(actorName, pos.x, pos.y + 1.f, pos.z - 4.f); // just north
    }

    void createSimpleXYZ(const std::string &actorName, float x, float y, float z) {
        bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::BaseProcMgr::CreateArg createArg;
        createArg.position = {x, y, z};
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = reinterpret_cast<CreateFunc*>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorMgr::requestCreateActorAsync::offset));
        engine::actor::ActorMgr* actorMgr = *reinterpret_cast<engine::actor::ActorMgr**>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorMgr::sInstance::offset));
        engine::actor::BaseProcMgr::Result result;

        bool ret = requestCreateActorAsync(actorMgr, actorName.c_str(), createArg, nullptr, engine::actor::CreatePriority::High, nullptr, nullptr, nullptr, false, &result, nullptr);

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "actor::createSimple(%s, %f, %f, %f) -> %d, code %d", actorName.c_str(), x, y, z, ret?1:0, (u32)result);
        svcOutputDebugString(buf, strlen(buf));
    }

    void createAndWatch(size_t slotIndex, const std::string &actorName) {
        if (lotuskit::script::globals::ResidentActors::Player == nullptr) {
            return createAndWatchXYZ(slotIndex, actorName, 0, 0, 0);
        }
        auto pos = lotuskit::script::globals::ResidentActors::Player->mPosition;
        createAndWatchXYZ(slotIndex, actorName, pos.x, pos.y + 1.f, pos.z - 4.f); // just north
    }

    void createAndWatchXYZ(size_t slotIndex, const std::string &actorName, float x, float y, float z) {
        bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::BaseProcMgr::CreateArg createArg;
        createArg.position = {x, y, z};
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = reinterpret_cast<CreateFunc*>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorMgr::requestCreateActorAsync::offset));
        engine::actor::ActorMgr* actorMgr = *reinterpret_cast<engine::actor::ActorMgr**>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorMgr::sInstance::offset));
        engine::actor::BaseProcMgr::Result result;

        engine::actor::PreActor* preactor = nullptr;

        //auto& slot = lotuskit::ActorWatcher::slots[slotIndex];
        bool ret = requestCreateActorAsync(actorMgr, actorName.c_str(), createArg, nullptr /* &slot.createWatcher */, engine::actor::CreatePriority::High, nullptr, nullptr, nullptr, false, &result, &preactor);
        //if (slot.createWatcher.watcher != nullptr) { watcherStatus = slot.createWatcher.watcher->status; }

        if (preactor != nullptr) {
            // ActorWatcher::calc() takes over waiting for actor
            lotuskit::ActorWatcher::assignSlotPreActor(slotIndex, preactor);
        }

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "actor::createAndWatch(%d, %s, %f, %f, %f) -> %d, code %d, preactor %p", slotIndex, actorName.c_str(), x, y, z, ret?1:0, (u32)result, preactor);
        svcOutputDebugString(buf, strlen(buf));
    }

    void setPosXYZ(ActorBase* actor, float x, float y, float z) {
        setPosRot(actor, sead::Vector3f{x, y, z}, actor->mRotation);
    }
    void setRot9(ActorBase* actor, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
        setPosRot(actor, actor->mPosition, sead::Matrix33f{m00, m01, m02, m10, m11, m12, m20, m21, m22});
    }
    void setPosRot39(ActorBase* actor, float x, float y, float z, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
        setPosRot(actor, sead::Vector3f{x, y, z}, sead::Matrix33f{m00, m01, m02, m10, m11, m12, m20, m21, m22});
    }
    void setPosRot(ActorBase* actor, sead::Vector3f pos, sead::Matrix33f rot) {
        ForceSetMatrixArg arg{pos, rot};
        ForceSetMatrixFunc* forceSetMatrix = reinterpret_cast<ForceSetMatrixFunc*>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorBase::forceSetMatrix::offset));
        u32 usuallyZeroFlag = 0;
        forceSetMatrix(actor, &arg, usuallyZeroFlag);
    }

    phive::RigidBodyEntity* getMainRigidBody(ActorBase* actor) {
        auto physCmp = actor->getPhysicsComponent();
        if (physCmp && physCmp->controllerSet && physCmp->controllerSet->mainRigidBody) {
            return physCmp->controllerSet->mainRigidBody;
        }
        return nullptr;
    }

}
