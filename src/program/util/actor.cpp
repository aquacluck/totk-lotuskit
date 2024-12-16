#include "ActorWatcher.hpp"
#include "util/actor.hpp"
#include "syms_merged.hpp"
#include "structs/bbBlackboard.hpp"
#include "structs/engineActor.hpp"
#include "script/globals.hpp"
#include <prim/seadSafeString.h>

namespace lotuskit::util::actor {
    using CreateFunc = bool (engine::actor::ActorInstanceMgr*, const sead::SafeString&, const engine::actor::ActorMgr::CreateArg&,
                             engine::actor::CreateWatcherRef*, engine::actor::CreatePriority, engine::actor::PreActor*,
                             engine::actor::ActorFile*, sead::Function*, bool, engine::actor::ActorMgr::Result*, engine::actor::PreActor**);

    void createSimple(const std::string &actorName) {
        if (lotuskit::script::globals::ResidentActors::Player == nullptr) {
            return createSimpleXYZ(actorName, 0, 0, 0);
        }
        auto pos = lotuskit::script::globals::ResidentActors::Player->mPosition;
        createSimpleXYZ(actorName, pos.x + 10.f, pos.y + 5.f, pos.z + 10.f);
    }

    void createSimpleXYZ(const std::string &actorName, float x, float y, float z) {
        bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::ActorMgr::CreateArg createArg;
        createArg.position = {x, y, z};
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::ActorMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::ActorMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = reinterpret_cast<CreateFunc*>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorInstanceMgr::requestCreateActorAsync::offset));
        engine::actor::ActorInstanceMgr* actorInstanceMgr = *reinterpret_cast<engine::actor::ActorInstanceMgr**>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorInstanceMgr::sInstance::offset));
        engine::actor::ActorMgr::Result result;

        bool ret = requestCreateActorAsync(actorInstanceMgr, actorName.c_str(), createArg, nullptr, engine::actor::CreatePriority::High, nullptr, nullptr, nullptr, false, &result, nullptr);

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "actor::createSimple(%s, %f, %f, %f) -> %d, code %d", actorName.c_str(), x, y, z, ret?1:0, (u32)result);
        svcOutputDebugString(buf, strlen(buf));
    }

    void createAndWatch(u8 slotIndex, const std::string &actorName) {
        if (lotuskit::script::globals::ResidentActors::Player == nullptr) {
            return createAndWatchXYZ(slotIndex, actorName, 0, 0, 0);
        }
        auto pos = lotuskit::script::globals::ResidentActors::Player->mPosition;
        createAndWatchXYZ(slotIndex, actorName, pos.x + 10.f, pos.y + 5.f, pos.z + 10.f);
    }

    void createAndWatchXYZ(u8 slotIndex, const std::string &actorName, float x, float y, float z) {
        bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::ActorMgr::CreateArg createArg;
        createArg.position = {x, y, z};
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::ActorMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::ActorMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = reinterpret_cast<CreateFunc*>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorInstanceMgr::requestCreateActorAsync::offset));
        engine::actor::ActorInstanceMgr* actorInstanceMgr = *reinterpret_cast<engine::actor::ActorInstanceMgr**>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorInstanceMgr::sInstance::offset));
        engine::actor::ActorMgr::Result result;

        engine::actor::PreActor* preactor = nullptr;

        //auto& slot = lotuskit::ActorWatcher::slots[slotIndex];
        bool ret = requestCreateActorAsync(actorInstanceMgr, actorName.c_str(), createArg, nullptr /* &slot.createWatcher */, engine::actor::CreatePriority::High, nullptr, nullptr, nullptr, false, &result, &preactor);
        //if (slot.createWatcher.watcher != nullptr) { watcherStatus = slot.createWatcher.watcher->status; }

        if (preactor != nullptr) {
            // ActorWatcher::calc() takes over waiting for actor
            lotuskit::ActorWatcher::assignSlotPreActor(slotIndex, preactor);
        }

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "actor::createAndWatch(%d, %s, %f, %f, %f) -> %d, code %d, preactor %p", slotIndex, actorName.c_str(), x, y, z, ret?1:0, (u32)result, preactor);
        svcOutputDebugString(buf, strlen(buf));
    }
}
