#include "ActorWatcher.hpp"
#include "util/actor.hpp"
#include "structs/bbBlackboard.hpp"
#include "script/globals.hpp"
#include <prim/seadSafeString.h>

#include "HexDump.hpp"
#include "TextWriter.hpp"

namespace lotuskit::util::actor {
    using CreateFunc = bool (engine::actor::ActorMgr*, const sead::SafeString&, const engine::actor::BaseProcMgr::CreateArg&,
                             engine::actor::CreateWatcherRef*, engine::actor::CreatePriority, engine::actor::PreActor*,
                             engine::actor::ActorFile*, sead::Function*, bool, engine::actor::BaseProcMgr::Result*, engine::actor::PreActor**);

    void createSimple(const std::string &actorName) {
        if (lotuskit::script::globals::ResidentActors::Player == nullptr) {
            return createSimpleXYZ(actorName, 0, 0, 0);
        }
        auto pos = lotuskit::script::globals::ResidentActors::Player->mPosition;
        createSimpleXYZ(actorName, pos.x, pos.y + 1.f, pos.z - 4.f); // just north
    }

    void createSimpleXYZ(const std::string &actorName, float x, float y, float z) {
        createSimplePosRot(actorName, sead::Vector3f{x, y, z}, sead::Matrix33f{1,0,0, 0,1,0, 0,0,1});
    }

    void createSimplePos(const std::string &actorName, const sead::Vector3f &pos) {
        createSimplePosRot(actorName, pos, sead::Matrix33f{1,0,0, 0,1,0, 0,0,1});
    }

    void createSimplePosRot(const std::string &actorName, const sead::Vector3f &pos, const sead::Matrix33f &rot) {
        bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::BaseProcMgr::CreateArg createArg;
        createArg.position = pos;
        createArg.rotation = rot;
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UseRotation);
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = EXL_SYM_RESOLVE<CreateFunc*>("engine::actor::ActorMgr::requestCreateActorAsync");
        engine::actor::ActorMgr* actorMgr = *EXL_SYM_RESOLVE<engine::actor::ActorMgr**>("engine::actor::ActorMgr::sInstance");
        engine::actor::BaseProcMgr::Result result;

        bool ret = requestCreateActorAsync(actorMgr, actorName.c_str(), createArg, nullptr, engine::actor::CreatePriority::High, nullptr, nullptr, nullptr, false, &result, nullptr);

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "actor::createSimple(%s, pos(%f, %f, %f)) -> %d, code %d", actorName.c_str(), pos.x, pos.y, pos.z, ret?1:0, (u32)result);
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
        createAndWatchPosRot(slotIndex, actorName, sead::Vector3f{x, y, z}, sead::Matrix33f{1,0,0, 0,1,0, 0,0,1});
    }

    void createAndWatchPos(size_t slotIndex, const std::string &actorName, const sead::Vector3f &pos) {
        createAndWatchPosRot(slotIndex, actorName, pos, sead::Matrix33f{1,0,0, 0,1,0, 0,0,1});
    }

    void createAndWatchPosRot(size_t slotIndex, const std::string &actorName, const sead::Vector3f &pos, const sead::Matrix33f &rot) {
        bb::InitInfo<32> initInfo;
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Bow"}, sead::SafeString{"Weapon_Bow_032"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Shield"}, sead::SafeString{"Weapon_Shield_018"});
        //initInfo.setParam(sead::SafeString{"EquipmentUser_Weapon"}, sead::SafeString{"Weapon_Sword_124"});
        engine::actor::BaseProcMgr::CreateArg createArg;
        createArg.position = pos;
        createArg.rotation = rot;
        createArg.scale = { 1.f, 1.f, 1.f };
        createArg.blackboard_info = &initInfo;
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UsePosition);
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UseRotation);
        createArg.transform_flags.set(engine::actor::BaseProcMgr::CreateArg::TransformFlags::UseScale);

        CreateFunc* requestCreateActorAsync = EXL_SYM_RESOLVE<CreateFunc*>("engine::actor::ActorMgr::requestCreateActorAsync");
        engine::actor::ActorMgr* actorMgr = *EXL_SYM_RESOLVE<engine::actor::ActorMgr**>("engine::actor::ActorMgr::sInstance");
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
        nn::util::SNPrintf(buf, sizeof(buf), "actor::createAndWatch(%d, %s, pos(%f, %f, %f)) -> %d, code %d, preactor %p", slotIndex, actorName.c_str(), pos.x, pos.y, pos.z, ret?1:0, (u32)result, preactor);
        svcOutputDebugString(buf, strlen(buf));
    }

    void setPos(ActorBase* actor, const sead::Vector3f &pos) {
        setPosRot(actor, pos, actor->mRotation);
    }
    void setPosXYZ(ActorBase* actor, float x, float y, float z) {
        setPos(actor, sead::Vector3f{x, y, z});
    }
    void setRot(ActorBase* actor, const sead::Matrix33f &rot) {
        setPosRot(actor, actor->mPosition, rot);
    }
    void setRot9(ActorBase* actor, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
        setRot(actor, sead::Matrix33f{m00, m01, m02, m10, m11, m12, m20, m21, m22});
    }
    void setPosRot39(ActorBase* actor, float x, float y, float z, float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
        setPosRot(actor, sead::Vector3f{x, y, z}, sead::Matrix33f{m00, m01, m02, m10, m11, m12, m20, m21, m22});
    }
    void setPosRot(ActorBase* actor, const sead::Vector3f &pos, const sead::Matrix33f &rot) {
        sead::Matrix34f arg{rot, pos};
        setPosRot34(actor, arg);
    }
    void setPosRot34(ActorBase* actor, const sead::Matrix34f &posrot) {
        using impl_t = void (engine::actor::ActorBase*, const sead::Matrix34f&, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("engine::actor::ActorBase::forceSetMatrix");
        u32 usuallyZeroFlag = 0;
        impl(actor, posrot, usuallyZeroFlag);
    }

    phive::RigidBodyEntity* getMainRigidBody(ActorBase* actor) {
        auto physCmp = actor->getPhysicsComponent();
        if (physCmp && physCmp->controllerSet && physCmp->controllerSet->mainRigidBody) {
            return physCmp->controllerSet->mainRigidBody;
        }
        return nullptr;
    }

    phive::RigidBodyEntity* getRigidBodyByName(ActorBase* actor, const std::string& name) {
        using impl_t = phive::RigidBodyEntity* (ActorBase*, const sead::SafeString&);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("engine::actor::ActorBase::getRigidBodyEntityByName");
        return impl(actor, name.c_str());
    }

    as::Blackboard* getASBlackboard(ActorBase* actor) {
        // XXX traversing this in angelscript would safer with builtin null checks... but perf + more plumbing
        as::Blackboard* bb = actor->getASComponent()->mASResourceBinder.mpASControllerSet->mpCurrentController->mpBlackboard;
        return bb;
    }

    void dumpASBlackboard(ActorBase* actor, u32 dumpMode) {
        auto* const bb = getASBlackboard(actor);
        if (bb == nullptr) { return; }
        const BBDumpMode mode = static_cast<BBDumpMode>(dumpMode); // lol c++
        const char* actorName = actor->mName.cstr();
        char buf[512];

        const auto n_string = bb->getStringCount();
        if (mode == BBDumpMode::MEMBERS) {
            nn::util::SNPrintf(buf, sizeof(buf), "%s(%p).ASBlackboard.String[%d]:", actorName, actor, n_string);
        } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            nn::util::SNPrintf(buf, sizeof(buf), "enum class %s_StringASKey_FileIndex: u32 {", actorName);
        }
        svcOutputDebugString(buf, strlen(buf));
        for (auto i=0; i < n_string; i++) {
            const char* k = bb->getStringKeyByIndex(i);
            const sead::SafeString& v = bb->getStringByIndex(i);
            if (mode == BBDumpMode::MEMBERS) {
                nn::util::SNPrintf(buf, sizeof(buf), "ASBlackboard.String[%d] {%s: %s}", i, k, v.cstr());
            } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
                nn::util::SNPrintf(buf, sizeof(buf), "    %s = 0x%x,", k, i); // emit enum
            }
            svcOutputDebugString(buf, strlen(buf));
        }
        if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            svcOutputDebugString("};", 2); // close enum
        }

        const auto n_int = bb->getIntCount();
        if (mode == BBDumpMode::MEMBERS) {
            nn::util::SNPrintf(buf, sizeof(buf), "%s(%p).ASBlackboard.Int[%d]:", actorName, actor, n_int);
        } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            nn::util::SNPrintf(buf, sizeof(buf), "enum class %s_IntASKey_FileIndex: u32 {", actorName);
        }
        svcOutputDebugString(buf, strlen(buf));
        for (auto i=0; i < n_int; i++) {
            const char* k = bb->getIntKeyByIndex(i);
            const s32 v = bb->getIntByIndex(i);
            if (mode == BBDumpMode::MEMBERS) {
                nn::util::SNPrintf(buf, sizeof(buf), "ASBlackboard.Int[%d] {%s: %d}", i, k, v);
            } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
                nn::util::SNPrintf(buf, sizeof(buf), "    %s = 0x%x,", k, i); // emit enum
            }
            svcOutputDebugString(buf, strlen(buf));
        }
        if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            svcOutputDebugString("};", 2); // close enum
        }

        const auto n_float = bb->getFloatCount();
        if (mode == BBDumpMode::MEMBERS) {
            nn::util::SNPrintf(buf, sizeof(buf), "%s(%p).ASBlackboard.Float[%d]:", actorName, actor, n_float);
        } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            nn::util::SNPrintf(buf, sizeof(buf), "enum class %s_FloatASKey_FileIndex: u32 {", actorName);
        }
        svcOutputDebugString(buf, strlen(buf));
        for (auto i=0; i < n_float; i++) {
            const char* k = bb->getFloatKeyByIndex(i);
            const float v = bb->getFloatByIndex(i);
            if (mode == BBDumpMode::MEMBERS) {
                nn::util::SNPrintf(buf, sizeof(buf), "ASBlackboard.Float[%d] {%s: %f}", i, k, v);
            } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
                nn::util::SNPrintf(buf, sizeof(buf), "    %s = 0x%x,", k, i); // emit enum
            }
            svcOutputDebugString(buf, strlen(buf));
        }
        if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            svcOutputDebugString("};", 2); // close enum
        }

        const auto n_bool = bb->getBoolCount();
        if (mode == BBDumpMode::MEMBERS) {
            nn::util::SNPrintf(buf, sizeof(buf), "%s(%p).ASBlackboard.Bool[%d]:", actorName, actor, n_bool);
        } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            nn::util::SNPrintf(buf, sizeof(buf), "enum class %s_BoolASKey_FileIndex: u32 {", actorName);
        }
        svcOutputDebugString(buf, strlen(buf));
        for (auto i=0; i < n_bool; i++) {
            const char* k = bb->getBoolKeyByIndex(i);
            const s32 v = bb->getBoolByIndex(i);
            if (mode == BBDumpMode::MEMBERS) {
                nn::util::SNPrintf(buf, sizeof(buf), "ASBlackboard.Bool[%d] {%s: %d}", i, k, v);
            } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
                nn::util::SNPrintf(buf, sizeof(buf), "    %s = 0x%x,", k, i); // emit enum
            }
            svcOutputDebugString(buf, strlen(buf));
        }
        if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            svcOutputDebugString("};", 2); // close enum
        }

        const auto n_vec3f = bb->getVec3fCount();
        if (mode == BBDumpMode::MEMBERS) {
            nn::util::SNPrintf(buf, sizeof(buf), "%s(%p).ASBlackboard.Vec3f[%d]:", actorName, actor, n_vec3f);
        } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            nn::util::SNPrintf(buf, sizeof(buf), "enum class %s_Vec3fASKey_FileIndex: u32 {", actorName);
        }
        svcOutputDebugString(buf, strlen(buf));
        for (auto i=0; i < n_vec3f; i++) {
            const char* k = bb->getVec3fKeyByIndex(i);
            const sead::Vector3f& v = bb->getVec3fByIndex(i);
            if (mode == BBDumpMode::MEMBERS) {
                nn::util::SNPrintf(buf, sizeof(buf), "ASBlackboard.Vec3f[%d] {%s: (%f, %f, %f)}", i, k, v.x, v.y, v.z);
            } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
                nn::util::SNPrintf(buf, sizeof(buf), "    %s = 0x%x,", k, i); // emit enum
            }
            svcOutputDebugString(buf, strlen(buf));
        }
        if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            svcOutputDebugString("};", 2); // close enum
        }

        const auto n_ptr = bb->getPtrCount();
        if (mode == BBDumpMode::MEMBERS) {
            nn::util::SNPrintf(buf, sizeof(buf), "%s(%p).ASBlackboard.Ptr[%d]:", actorName, actor, n_ptr);
        } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            nn::util::SNPrintf(buf, sizeof(buf), "enum class %s_PtrASKey_FileIndex: u32 {", actorName);
        }
        svcOutputDebugString(buf, strlen(buf));
        for (auto i=0; i < n_ptr; i++) {
            const char* k = bb->getPtrKeyByIndex(i);
            const void* v = bb->getPtrByIndex(i);
            if (mode == BBDumpMode::MEMBERS) {
                nn::util::SNPrintf(buf, sizeof(buf), "ASBlackboard.Ptr[%d] {%s: %p}", i, k, v);
            } else if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
                nn::util::SNPrintf(buf, sizeof(buf), "    %s = 0x%x,", k, i); // emit enum
            }
            svcOutputDebugString(buf, strlen(buf));
        }
        if (mode == BBDumpMode::ENUMCLS_FILE_DECL) {
            svcOutputDebugString("};", 2); // close enum
        }

        //virtual const sead::SafeString& getFloatKeyByIndex(s32 index);
        //virtual const sead::SafeString& getFloatFilenameByIndex(s32 index);
        //virtual const sead::SafeString& getFloatDescriptionByIndex(s32 index);

    }

}
