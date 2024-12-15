#include "util/actor.hpp"
#include "syms_merged.hpp"

namespace lotuskit::util::actor {

    void testCreate() {
        //bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::ActorMgr::CreateArg createArg;
        createArg.position = {30.f, 1520.f, 1500.f}; // fuse
        //createArg.position = player->getPosition();
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = nullptr;
        //createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::ActorMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::ActorMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = reinterpret_cast<CreateFunc*>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorInstanceMgr::requestCreateActorAsync::offset));
        engine::actor::ActorInstanceMgr* actorInstanceMgr = *reinterpret_cast<engine::actor::ActorInstanceMgr**>(exl::util::modules::GetTargetOffset(sym::engine::actor::ActorInstanceMgr::sInstance::offset));
        engine::actor::ActorMgr::Result result;

        bool ret = requestCreateActorAsync(actorInstanceMgr, sead::SafeString{"Animal_Kokko_A"}, createArg, nullptr, engine::actor::CreatePriority::High, nullptr, nullptr, nullptr, false, &result, nullptr);

        char buf[1000];
        nn::util::SNPrintf(buf, sizeof(buf), "[testCreate] flag %d code %d", ret?1:0, (u32)result); // flag 1 code 0
        svcOutputDebugString(buf, strlen(buf));
    }

}
