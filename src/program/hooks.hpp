#pragma once
#include <utility>

#include "lib.hpp"
#include "nn/nn.h"
#include "nn/hid.h"
#include "nn/util.h"
#include "helpers/InputHelper.hpp"
#include "helpers/ConfigHelper.hpp"
#include "lib/util/ptr_path.hpp"
#include "lib/util/sys/mem_layout.hpp"
#include "structs.hpp"
#include "types.h"
#include "logger/Logger.hpp"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

// agl include
#include "common/aglDrawContext.h"
#include "lyr/aglLayer.h"
#include "lyr/aglRenderInfo.h"

// agl sym
#include "sym/agl_hacks.h"
#include "sym/lyr/aglLayer.h"

// sead include
#include "gfx/seadColor.h"
#include "gfx/seadDrawContext.h"
#include "gfx/seadTextWriter.h"
#include "gfx/seadViewport.h"
#include "math/seadVector.hpp"
#include "random/seadGlobalRandom.h"

// sead sym
#include "sym/gfx/seadTextWriter.h"
#include "sym/random/seadGlobalRandom.h"
#include "sym/random/seadRandom.h"

// engine sym
#include "sym/engine/actor/ActorInstanceMgr.h"
#include "sym/engine/actor/ActorMgr.h"
#include "sym/engine/actor/BaseProcCreateAndDeleteThread.h"
#include "sym/engine/hacks.h"
#include "sym/engine/module/VFRMgr.h"

// game sym
#include "sym/game/ai/execute/ExecutePlayerWhistle.h"
#include "sym/game/wm/WorldManagerModule.h"

// havok sym
#include "sym/hk/hacks.h"


// feature flags to minimize log noise and similar clutter/risk/etc
constexpr bool DO_RELATION_LOG = 1;
constexpr bool DO_PHYSICS_SPAM_LOG = 0;
constexpr bool DO_VFR_LOG = 0;
#define DO_XXX_ACTOR_CREATION_LOG 1

// global state owned by nnMainHook (at the bottom!)
struct nnMainHookState_t {
    Logger* main_logger;
};
nnMainHookState_t* nnMainHookState = nullptr;

extern ModCommand_Savestate_ActorPos g_ModCommand_Savestate_ActorPos[4]; // XXX hooks.hpp logger.cpp trash glue
extern ModCommand_ActorWatcher g_ModCommand_ActorWatcher[4]; // XXX hooks.hpp logger.cpp trash glue

const float PLAYER_PHYSICS_HEIGHT_OFFSET = 0.9; // constant offset between pos32/pos64
ActorBase* Player = nullptr;
PreActor* Player_PreActor = nullptr;
hknpMotion* Player_physics = nullptr;

ActorMgr* ActorMgr_instance = nullptr;
ActorBase* PlayerCamera = nullptr;
ActorBase* EventCamera = nullptr;
ActorBase* Parasail;
ActorBase* Dm_Npc_RevivalFairy;
ActorBase* CarryBox;
ActorBase* ThrowBox;


HOOK_DEFINE_TRAMPOLINE(LoggerConnectOnWhistleHook) {
    static const ptrdiff_t s_offset = sym::game::ai::execute::ExecutePlayerWhistle::enterImpl_;

    static void Callback(void* param) {
        auto main_logger = nnMainHookState->main_logger;
        main_logger->log(NS_DEFAULT_TEXT, R"("trying frontend connect() ")");
        main_logger->connect();
        main_logger->logf(NS_DEFAULT_TEXT, R"("main_offset %p")", exl::util::GetMainModuleInfo().m_Total.m_Start);

        sead::GlobalRandom* grand = *exl::util::pointer_path::FollowSafe<sead::GlobalRandom*, sym::sead::GlobalRandom::sInstance>();
        main_logger->logf(NS_DEFAULT_TEXT, R"("great %p %d")", grand, grand->getU32());

        Orig(param);
    }
};

HOOK_DEFINE_TRAMPOLINE(ActorRelationAddHook) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorMgr::registerActorRelation;

    static u32 Callback(ActorMgr &actor_mgr, ActorBase &parentBase, ActorBase &childBase) {
        auto main_logger = nnMainHookState->main_logger;
        IActor *parent = &(parentBase.mIActor);
        IActor *child = &(childBase.mIActor);
        ActorMgr_instance = &actor_mgr;
        u8 is_skip = 0;

        // resolve "via next relation" actor selection
        for (u8 i=1; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_pending_selection || cmd.selection_type != 2) { continue; }
            if (cmd.actor_selector_parent_relation == &parentBase) {
                cmd.actor = &childBase;
                cmd.is_pending_selection = false;
                cmd.is_publishing_selection = true;
            } else if (cmd.actor_selector_parent_relation == &childBase) {
                // not sure we really care about parent/child here? use either
                cmd.actor = &parentBase;
                cmd.is_pending_selection = false;
                cmd.is_publishing_selection = true;
            }
        }

        if(!strcmp(parent->name, "PlayerCamera")) { PlayerCamera = &parentBase; }
        if(!strcmp(parent->name, "EventCamera")) { EventCamera = &parentBase; }

        if(!strcmp(parent->name, "Player")) {
            if(Player != &parentBase) {
                main_logger->logf(NS_DEFAULT_TEXT, R"("Reassigning Player %p -> %p")", Player, &parentBase);
                Player = &parentBase;
                g_ModCommand_ActorWatcher[0].actor = Player;
                g_ModCommand_ActorWatcher[0].is_pending_selection = false;
                g_ModCommand_ActorWatcher[0].is_publishing_selection = true;
            }

            // Required relations (will retry loop at boot)
            if(!strcmp(child->name, "Parasail")) { Parasail = &childBase; } // Removable (glider remains at location Player opened it)
            if(!strcmp(child->name, "Dm_Npc_RevivalFairy")) { Dm_Npc_RevivalFairy = &childBase; } // Removable (fairy remains at location Player triggered it)
            if(!strcmp(child->name, "CarryBox")) { CarryBox = &childBase; }
            if(!strcmp(child->name, "ThrowBox")) { ThrowBox = &childBase; }

            // Optional relations applied at boot
            /*
            if(DO_RELATION_SKIP) {
                // No crash but haven't seen their effects
                if(!strcmp(child->name, "Obj_OneTouch_Connection")) { is_skip = 1; } // idk what it means
                if(!strcmp(child->name, "WakeBoardRope")) { is_skip = 1; } // TODO go ride a seal? prob mild visual

                // Can't observe these in title scene, but skipping them in-game leaves model behind like overload
                if(!strcmp(child->name, "Armor_Default_Upper")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_Default_Lower")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_999_Head")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_Default_Extra_01")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_Default_Extra_00")) { is_skip = 1; }
                if(!strcmp(child->name, "Accessory_Battery")) { is_skip = 1; }
                if(!strcmp(child->name, "Accessory_SupportDevice")) { is_skip = 1; }
                if(!strcmp(child->name, "PlayerRaulHand")) { is_skip = 1; } // Rare missing arm
                if(!strcmp(child->name, "Weapon_Quiver_001")) { is_skip = 1; }
            }
            */
        }
        if (is_skip) {
            if (DO_RELATION_LOG) {
                main_logger->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op_skipped": 1, "op": "RelationAdd", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            }
            // XXX what is 2724636224? what exactly is skipped here -vs- still handled at callsites?
            return 2724636224; // Report success but do not attach (idk where this func is in the process, but bailing seems to do overload things)
        }
        u32 result = Orig(actor_mgr, parentBase, childBase);

        if (DO_RELATION_LOG) {
            if (result) {
                main_logger->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op": "RelationAdd", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            } else {
                // XXX can this really fail?
                main_logger->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op_failed": 1, "op": "RelationAdd", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            }
        }

        return result;

        /*
        // eg iterate over children
        if (paren->.count > 0) {
            std::string children;
            children.reserve(1024);
            for (u32 i = 0; i < paren->.count; ++i) {
                ActorBase * child = GetChild(actor_mgr, (u32)parent->array[i], *parent);
                if (child != nullptr) {
                    children.append(child->name, strlen(child->name));
                    children.append(" ");
                }
            }
        }
        */
    }
};

// I think the parent and child can sometimes be flipped in this function?
HOOK_DEFINE_TRAMPOLINE(ActorRelationRemoveHook) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorMgr::removeActorRelation;

    static u32 Callback(ActorMgr &actor_mgr, ActorBase &parentBase, ActorBase &childBase) {
        auto main_logger = nnMainHookState->main_logger;
        IActor *parent = &(parentBase.mIActor);
        IActor *child = &(childBase.mIActor);


        if(!strcmp(parent->name, "Player") && !strcmp(child->name, "Weapon_Sword_044")) { // Tree Branch
        //if(!strcmp(parent->name, "Player") && !strcmp(child->name, "Weapon_Lsword_020")) { // Rusty Claymore
        //if(!strcmp(parent->name, "Player") && !strcmp(child->name, "Weapon_Sword_021")) { // Rusty Broadsword
            // Forcibly remove required relations
            //Orig(actor_mgr, parentBase, *Parasail);
            //Orig(actor_mgr, parentBase, *Dm_Npc_RevivalFairy);
            //Orig(actor_mgr, parentBase, *CarryBox); // softlock next time opening inventory after creating the next bundle
            //Orig(actor_mgr, parentBase, *ThrowBox); // crash
        }


        u32 result = Orig(actor_mgr, parentBase, childBase);

        if (DO_RELATION_LOG) {
            if (result) {
                main_logger->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op": "RelationRemove", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            } else {
                main_logger->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op_failed": 1, "op": "RelationRemove", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            }
        }

        return result;
    }
};


HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static const auto s_offset = sym::game::wm::WorldManagerModule::baseProcExe;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        auto main_logger = nnMainHookState->main_logger;
        InputHelper::updatePadState();

        if (Player == nullptr) {
            // early return to wait for the first relation to populate Player. sloppy but it's just bootup, not much going on anyways
            return Orig(self, param_2, param_3, param_4, wmmodule, param_6);
        }

        // Parse any pending commands on the wire
        main_logger->tryRecvCommandMainLoop(); // XXX filthy hack, nonblocking recv

        // Command: actor paste
        // TODO efficiently know when+where work items are pending in this arr
        auto &cmd = g_ModCommand_Savestate_ActorPos[0];
        if(cmd.is_active_request && strcmp("Player", cmd.actor_selector_named) == 0) {
            // FIXME rotation doesn't restore, it only affects the map.
            // FIXME model often doesn't warp until movement, similar to freezing coords

            Player->mPosition = cmd.pos32;
            Player->mRotationMatrix = cmd.rot;
            Player_physics->m_centerOfMass = cmd.pos64; // FIXME spooky assignments, just copy members
            cmd.is_active_request = false;

            main_logger->logf(NS_SAVESTATE, R"({"actor_paste": ["%s"], "actor_props": [{"pos64": [%f, %f, %f], "pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] }] })",
                cmd.actor_selector_named,
                cmd.pos64.X, cmd.pos64.Y, cmd.pos64.Z,
                cmd.pos32.X, cmd.pos32.Y, cmd.pos32.Z,
                cmd.rot.m11, cmd.rot.m12, cmd.rot.m13, cmd.rot.m21, cmd.rot.m22, cmd.rot.m23, cmd.rot.m31, cmd.rot.m32, cmd.rot.m33
            );
        }

        // Resolve pending actor selection via preactors
        // TODO efficiently pick slots needing resolve work
        // TODO check for entries that take too long to resolve -> hack to avoid checking this every frame for those cases (or just finish the create actor hooking and stop polling)
        for (u8 i=1; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_pending_selection || cmd.selection_type == 0 || cmd.preactor == nullptr) { continue; }
            if (cmd.preactor->mpActor != nullptr) {
                // FIXME this traversal is broken on 1.0.0? bad actor pointer
                // eg main_logger->logf(NS_DEFAULT_TEXT, R"({"player preactor": "%p -> %p"})", Player_PreActor, Player_PreActor->mpActor);
                cmd.actor = cmd.preactor->mpActor;
                cmd.is_pending_selection = false;
                cmd.is_publishing_selection = true;
            }
        }

        // Logging+instrumentation in this thread comes after ^ command proc

        for (u8 i=0; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_publishing_selection || cmd.actor == nullptr) { continue; }
            const auto actor = cmd.actor;
            const auto rot = actor->mRotationMatrix;
            hknpMotion* motion = cmd.tracked_motion;

            const char* ns = i == 0 ? NS_ACTOR_PLAYER :
                             i == 1 ? NS_ACTOR_SLOT_1 :
                             i == 2 ? NS_ACTOR_SLOT_2 :
                             i == 3 ? NS_ACTOR_SLOT_3 : NS_DEFAULT_TEXT;

            if (motion != nullptr) {
                const auto pos64 = motion->m_centerOfMass;
                main_logger->logf(ns, R"({"pos64": [%f, %f, %f], "rot_physics": [%f, %f, %f, %f], "physics_ang_vel": [%f, %f, %f], "physics_vel": [%f, %f, %f], "inertia": [%f, %f, %f, %f], "pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] })",
                    // hknpMotion
                    pos64.X, pos64.Y, pos64.Z,
                    motion->m_orientation.A, motion->m_orientation.B, motion->m_orientation.C, motion->m_orientation.D,
                    motion->m_angularVelocityLocalAndSpeedLimit.X, motion->m_angularVelocityLocalAndSpeedLimit.Y, motion->m_angularVelocityLocalAndSpeedLimit.Z,
                    motion->m_linearVelocityAndSpeedLimit.X, motion->m_linearVelocityAndSpeedLimit.Y, motion->m_linearVelocityAndSpeedLimit.Z,
                    motion->m_inverseInertia[0], motion->m_inverseInertia[1], motion->m_inverseInertia[2], motion->m_inverseInertia[3],
                    // Actor
                    actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );

                // TODO try writing angles, velocities, prev+now, inertia, ... just so we know what happens
                // TODO try blanking out / early returning / etc selectively to see how much we can fix movement... needs actor<->phive<->hknpBody/Shape<->hknpMotion / similar association?
            } else {

                // Actor only
                main_logger->logf(ns, R"({"pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] })",
                    actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );
            }
        }

        if (DO_VFR_LOG) {
            // "raw" time is affected by bullet time and not much else (not ascend, recall, textboxes, textbox logs, pausing, memories)
            // bt drops to 0.05 0.001667, then 4 interpolated frames back up to normal 1.0 0.033333 when it ends. This can stack/multiply with perf hits:
            // Both raw and cooked are affected by performance:
            // On 1.2.1 it will switch to 1.5 0.05 during heavy load. That's 1.5x the normal duration, or 0.05 = 1/20 seconds.
            // 1.0 all seems the same. I thought only 1.0 was locked between 20fps and 30fps?
            VFRMgr* vfr_mgr = *exl::util::pointer_path::FollowSafe<VFRMgr*, sym::engine::module::VFRMgr::sInstance>();
            main_logger->logf(NS_VFRMGR, R"({"vfr_frame": [%f, %f, %f, %f, %f] })", vfr_mgr->mRawDeltaFrame, vfr_mgr->mRawDeltaTime, vfr_mgr->mDeltaFrame, vfr_mgr->mDeltaTime, vfr_mgr->mIntervalValue);
            for(int i = 0; i < 16; i++) {
                // default all slots: 1.0 nan
                // during bt: 0 -> 0.05 0.05, 3 -> 0.35 0.35
                main_logger->logf(NS_VFRMGR, R"({"vfr_timespeedmultiplier_i": %d, "pair": [%f, %f] })", i, vfr_mgr->mTimeSpeedMultipliers[i].mValue, vfr_mgr->mTimeSpeedMultipliers[i].mTarget);
            }
        }


        if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Backquote)) {
            main_logger->log(NS_DEFAULT_TEXT, "\"oink\"");
        }


        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};


// Thanks to MaxLastBreath for figuring this out in Ultracam -- similar approach here
HOOK_DEFINE_TRAMPOLINE(TryGetPlayerPhysicsPosPtrHook) {
    static const ptrdiff_t s_offset = sym::hkUNKNOWN::UNKNOWN_applyMotion;

    static void Callback(void* self, void* param_2, hknpMotion* hkmotion, hknpMotion* motion_pool) {
        auto main_logger = nnMainHookState->main_logger;

        if (DO_PHYSICS_SPAM_LOG) {
            // frontend proxy(?) can't keep up with this
            //     - TODO ez runtime work+log toggles/subscriptions instead of defines
            //     - TODO e2e blocking: optionally stop the world until the frontend ui reports render, slow+accurate mode aside from concurrency+timing+etc issues
            // motion_pool[0] is something else probably? then usually a few which rarely/never update, then Player/etc usually starts from there
            main_logger->logf(NS_PHYSICS_SPAM, R"("param_2: %p, hkmotion: %p, motion_pool: %p")", param_2, hkmotion, motion_pool);
        }

        // This seems related to object movement while touching terrain? Is this for collision or movement or ?
        // Early return here interrupts a lot of movement, but it's inconsistent+choppy similar to freezing coords.
        // Eg air movement, standing on wings, even inside shrines has free movement. Safe ground handler?
        //return Orig(self, param_2, hkmotion, motion_pool);

        //main_logger->log("TryGetPlayerPhysicsPosPtrHook: %p %p %p %p", self, param_2, hkmotion, motion_pool);
        Vector3d* pos64 = &(hkmotion->m_centerOfMass);

        // Player + default relations are initialized before this
        Vector3f pos32 = Player->mPosition;

        if (Player_physics != nullptr) {
            // Nothing to do, we have it
            return Orig(self, param_2, hkmotion, motion_pool);

            // TODO find the new instance if needed, but not like this
            if(abs(pos32.X - Player_physics->m_centerOfMass.X) > 0.1 && abs(pos32.Y + PLAYER_PHYSICS_HEIGHT_OFFSET - Player_physics->m_centerOfMass.Y) > 0.1 && abs(pos32.Z - Player_physics->m_centerOfMass.Z) > 0.1) {
                Player_physics = nullptr;
            }
        }

        //main_logger->log("Player pos32: %f %f %f", pos32.X, pos32.Y, pos32.Z);
        //main_logger->log("candid pos64: %f %f %f", pos64->X, pos64->Y, pos64->Z);
        if(abs(pos32.X - pos64->X) < 0.01 && abs(pos32.Y + PLAYER_PHYSICS_HEIGHT_OFFSET - pos64->Y) < 0.01 && abs(pos32.Z - pos64->Z) < 0.01) {
            // This is the first and only very-close physics object (if that's what this is) during bootup. It's always 0.9m off.
            // FIXME reallocated/??? sometimes... after several trapped-in-water voids at (0 0 0) I got this
            main_logger->logf(NS_DEFAULT_TEXT, R"("Reassigning Player_physics %p -> %p")", Player_physics, hkmotion);
            Player_physics = hkmotion;
            g_ModCommand_ActorWatcher[0].tracked_motion = Player_physics;
        }

        Orig(self, param_2, hkmotion, motion_pool);
    }
};

HOOK_DEFINE_TRAMPOLINE(HookRNG_sead_u32) {
    static const ptrdiff_t s_offset = sym::sead::Random::getU32;

    inline static bool do_log = false;
    inline static bool do_stub = false;
    inline static u32 stub_value = 420;

    static u32 Callback(void* param) { // sead::Random*
        u32 ret;
        if (do_stub) {
            ret = stub_value;
            if (do_log) {
                nnMainHookState->main_logger->logf(NS_DEFAULT_TEXT, R"("sead::Random::u32(%p) ~~ %d")", param, ret);
            }
        } else {
            ret = Orig(param);
            if (do_log) {
                nnMainHookState->main_logger->logf(NS_DEFAULT_TEXT, R"("sead::Random::u32(%p) => %d")", param, ret);
            }
        }
        return ret;
    }
};
//bool HookRNG_sead_u32::do_log = false;
//bool HookRNG_sead_u32::do_stub = false;
//u32 HookRNG_sead_u32::stub_value = 420;


#if DO_XXX_ACTOR_CREATION_LOG
HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook1) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::createActorAndConnectSync;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,undefined **param_8,undefined *param_9,
        undefined8 param_10,void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,
        undefined8 *param_14,undefined4 param_16,undefined4 param_17,undefined8 param_15, undefined param_19) {

        auto main_logger = nnMainHookState->main_logger;
        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_sync1"})", *param_4); // only Obj_OneTouch_Connection, no overlap

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook2) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::createActorAndConnectSync__2;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,undefined **param_8,undefined *param_9,
        undefined8 param_10,void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,
        undefined8 *param_14,undefined4 param_16,undefined4 param_17,undefined8 param_15, undefined param_19) {

        auto main_logger = nnMainHookState->main_logger;
        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_idk2"})", *param_4); // XXX never hit?

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook3) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::createActorAndConnectSyncImpl;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,long param_8,long param_9,undefined8 param_10,
        void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,long *param_14,
        undefined4 param_16,undefined4 param_17,undefined8 param_15,undefined param_19) {

        // only relation names (eg fused rocks, fused wings, but weird stuff too like quickmenu-icon-screenshotting and pause-actor instantiations). no overlap with other hooks
        auto main_logger = nnMainHookState->main_logger;
        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_relation"})", *param_4);

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook4) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::requestCreateActorAsync;

    static long Callback
        (void* actorInstanceMgr, char** actorName, Vector3f* pos, void* param_4, undefined4 param_5, PreActor* preActor,
        void* actorObserver, void* param_8, u8 param_9, undefined4* param_10, PreActor** destPreActor) {

        auto main_logger = nnMainHookState->main_logger;

        // inject our own dest pointer if needed
        PreActor* pa = nullptr;
        if (destPreActor == nullptr) {
            destPreActor = &pa;
        }

        // async, most actors
        auto ret = Orig(actorInstanceMgr, actorName, pos, param_4, param_5, preActor, actorObserver, param_8, param_9, param_10, destPreActor);

        pa = *destPreActor;
        if (pa == nullptr) {
            // eg TerrainEditCamera returns without assigning a preactor (disabled? check ret value?)
            main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create", "pos": [%f, %f, %f], "preactor": "%p" })",
                *actorName,
                pos->X, pos->Y, pos->Z,
                pa
            );
        } else {
            main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create", "pos": [%f, %f, %f], "preactor": "%p", "preactor_pos": [%f, %f, %f], "actor": "%p" })",
                *actorName,
                pos->X, pos->Y, pos->Z, // rarely has placement
                pa,
                pa->mPosition.X, pa->mPosition.Y, pa->mPosition.Z, // usually has placement
                pa->mpActor // always nullptr
            );
        }

        if (!strcmp(*actorName, "Player")) {
            Player_PreActor = pa;
            main_logger->logf(NS_DEFAULT_TEXT, R"("Reassigning Player preactor %p")", Player_PreActor);
            g_ModCommand_ActorWatcher[0].preactor = Player_PreActor;
        }

        // resolve "next spawn" actor selection
        for (u8 i=1; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_pending_selection || cmd.selection_type != 1 || cmd.preactor != nullptr || pa == nullptr) { continue; }
            cmd.preactor = pa;
            // cmd.is_pending_selection = true; // still pending selection: now we check every frame for mpActor to be set
        }

        return ret;
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook5) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::requestCreateActorSync;

    static long Callback(long **param_1,char **param_2,long param_3,long param_4,long param_5,undefined8 param_6, int *param_7) {
        // few actors at bootup: ReactionHit ReactionField Chemical EffectUI SpecialPower WakeBoardRope
        auto main_logger = nnMainHookState->main_logger;
        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_sync"})", *param_2);
        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7);
    }
};

HOOK_DEFINE_INLINE(TestCreateActorHookImplEnter) {
    static const ptrdiff_t s_offset = 0x0244a638; //121
    static void Callback(exl::hook::InlineCtx* ctx) {
        auto main_logger = nnMainHookState->main_logger;
        main_logger->logf(NS_ACTOR, R"({"name": "top ", "actor_op": "calc_create", "_": "%p %p %p"})", ctx->X[0], ctx->X[1], ctx->X[2]);
    }
};
HOOK_DEFINE_INLINE(TestCreateActorHookImpl) {
    static const ptrdiff_t s_offset = sym::engine::actor::BaseProcCreateAndDeleteThread::calc_create; // 0x0244a80c 121
    static void Callback(exl::hook::InlineCtx* ctx) {
        auto main_logger = nnMainHookState->main_logger;
        //XXX how stable are inline register accesses like this across versions? (eg originally x0 param but moved to x19 before our hook)
        //    annotate SymbolType.INSTRUCTION with named registers if this becomes a problem.
        //void* self = ctx->X[19]; // param_1
        //lVar16 = (BaseProcCreateAndDeleter *)param_1[0x1b];

        // (param_1[0x1b] + 0x10)   ( (param_1[0x1b] + 0x10),  lVar9 )
        // BaseProcCreaterAndDeleter* thing; // idk type? but this holds a mCreateProcFunction. TODO also idk how to traverse IFunction, is it really just IFunction.invoke at +0? print %p at runtime or find the vtable or name or something idk how to follow this
        // thing->mCreateProcFunction(thing->mCreateProcFunction, BaseProcCreateRequest *)

        char* neatlog =*(char**)(ctx->X[19]+(0x21*8)); // vanilla snprintf: f"{BaseProcCreateRequest->mName}({BaseProcCreateRequest*})"
        main_logger->logf(NS_ACTOR, R"({"name": "nope", "actor_op": "calc_create", "_": "%p %s"})", ctx->X[19], neatlog);
    }
};
HOOK_DEFINE_INLINE(TestDeleteActorHookImpl) {
    static const ptrdiff_t s_offset = sym::engine::actor::BaseProcCreateAndDeleteThread::calc_delete;

    static void Callback(exl::hook::InlineCtx* ctx) {
        //void* self = (void*)(ctx->X[0]);
        //void* idk = (void*)(ctx->X[1]);
        //char* name = (char*)(self + 0x21);
        //auto main_logger = nnMainHookState->main_logger;
        //main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "calc_delete"})", name);
    }
};
#endif


// this function actually exists in 1.0.0 but is inlined in later versions so I just recreated it for simplicity
// 0x71006fdc84 on 1.0.0
/*
ActorBase * GetChild(ActorMgr& mgr, u32 index, ActorBase &parent) {
    u32 idx = index & 0xFFFF;
    if (mgr.count <= idx) {
        return mgr.array[0].child;
    }
    if (mgr.array[idx].parent == &parent) {
        return mgr.array[idx].child;
    }

    return nullptr;
}
*/


struct GraphicsModuleCreateArg {
    char _whatever[0xb4c];
    int value0;
    char _whatever2[0x10];
    int value1;
};

// used for DebugDrawEnsureFont
HOOK_DEFINE_INLINE(StealHeap) {
    static const ptrdiff_t s_offset = sym::engine::steal_heap; // hacks
    inline static sead::Heap* stolen_heap = nullptr;
    static void Callback(exl::hook::InlineCtx* ctx) {
#ifdef TOTK_100
        // TODO register annotation/aliasing instead
        stolen_heap = reinterpret_cast<sead::Heap*>(ctx->X[19]);
#else
        stolen_heap = reinterpret_cast<sead::Heap*>(ctx->X[22]);
#endif
    }
};

// used for DebugDrawEnsureFont
HOOK_DEFINE_INLINE(GetCreateArg) {
    static const ptrdiff_t s_offset = sym::agl::create_arg; // hacks
    inline static GraphicsModuleCreateArg* create_arg = nullptr;
    static void Setup(void) {
        create_arg = new GraphicsModuleCreateArg();
    }
    static void Callback(exl::hook::InlineCtx* ctx) {
        create_arg->value0 = reinterpret_cast<GraphicsModuleCreateArg*>(ctx->X[1])->value0;
        create_arg->value1 = reinterpret_cast<GraphicsModuleCreateArg*>(ctx->X[1])->value1; // nvnBufferBuilderSetStorage?
    }
};

HOOK_DEFINE_INLINE(DebugDrawEnsureFont) {
    static const ptrdiff_t s_offset = sym::agl::lyr::RenderDisplay::drawLayer_::ensure_font; // hacks

    inline static nn::os::MutexType* fontMutex = nullptr;
    inline static void** sDefaultFont = nullptr;

    static void Setup(void) {
        fontMutex = new nn::os::MutexType();
        nn::os::InitializeMutex(fontMutex, true, 0);

        sDefaultFont = exl::util::pointer_path::FollowSafe<void*, sym::agl::default_font>(); // hacks
    }

    static void Callback(exl::hook::InlineCtx* ctx) {
        // ignore other layers
        auto* layer = (agl::lyr::Layer*)(ctx->X[21]);
        if (strncmp("Tool 2D", layer->mLayerName.cstr(), 0x20) != 0) {
            return;
        }

        // init font if needed
        if (*sDefaultFont == nullptr) {
            nn::os::LockMutex(fontMutex);
            if (*sDefaultFont == nullptr) {
                auto main_logger = nnMainHookState->main_logger;
                main_logger->log(NS_DEFAULT_TEXT, R"("init default font ")");

                void(*InitDebugDrawers)(sead::Heap*, GraphicsModuleCreateArg&) = nullptr;
                void** tmp = (void**)(&InitDebugDrawers);
                *tmp = exl::util::pointer_path::FollowSafe<void*, sym::agl::init_debug_drawers>(); // hacks
                InitDebugDrawers(StealHeap::stolen_heap, *GetCreateArg::create_arg);

                if (*sDefaultFont == nullptr) {
                    main_logger->log(NS_DEFAULT_TEXT, R"("init default font fail")");
                    return;
                }
            }
            nn::os::UnlockMutex(fontMutex);
        }

        // allow layer to draw
        ctx->W[8] = 0x28; // satisfy check to call agl::lyr::Layer::drawDebugInfo_
        layer->mRenderFlags |= 1 << 13; // satisfy check inside vanilla drawDebugInfo_
    }
};


class TextWriterExt: public sead::TextWriter {
    public:
    void getCursorFromTopLeftImpl(sead::Vector2f* pos) const {
        // always 720p sooooo just make it work
        pos->x = mCursor.x + 640.0;
        pos->y = 360.0 - mCursor.y;
    }
    void pprintf(sead::Vector2f &pos, const char* fmt, auto&&... args) {
        // pretty print: text shadow drawn first
        this->mColor.r = 0.0; this->mColor.g = 0.0; this->mColor.b = 0.0; this->mColor.a = 1.0; // black
        this->setCursorFromTopLeft(pos);
        this->printf(fmt, std::forward<decltype(args)>(args)...); // javascript lookin ass bullshit

        // main draw
        this->mColor.r = 0.85; this->mColor.g = 0.85; this->mColor.b = 0.85; this->mColor.a = 1.0; // white ish
        pos.x -= 1.0;
        pos.y -= 1.0;
        this->setCursorFromTopLeft(pos);
        this->printf(fmt, std::forward<decltype(args)>(args)...);

        // remember position for future calls
        this->getCursorFromTopLeftImpl(&pos);
        pos.x += 1.0;
        pos.y += 1.0;
    }
};


HOOK_DEFINE_TRAMPOLINE(DebugDrawImpl) {
    static const ptrdiff_t s_offset = sym::agl::lyr::Layer::drawDebugInfo_;
    static void Callback(agl::lyr::Layer* layer, const agl::lyr::RenderInfo& info) {
        // draw onto the given layer, always Tool 2D -- we would be given many layers if they weren't ignored above

        // XXX do we really need to do this every time? weird its a static. maybe we can hold onto one instead
        auto* sead_draw_ctx = dynamic_cast<sead::DrawContext*>(info.draw_ctx);
        sead::TextWriter::setupGraphics(sead_draw_ctx);
        sead::TextWriter _writer(sead_draw_ctx, info.viewport);
        TextWriterExt* writer = (TextWriterExt*)(&_writer);

        // screen is always represented as 1280x720, upscaled for 1080p
        sead::Vector2f text_pos;
        text_pos.x = 2.0;
        text_pos.y = 2.0;

        // 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast
        writer->mScale.x = 0.8;
        writer->mScale.y = 0.8;

        for (u8 i=0; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_publishing_selection || cmd.actor == nullptr) { continue; }
            const auto actor = cmd.actor;
            const auto rot = actor->mRotationMatrix;
            hknpMotion* motion = cmd.tracked_motion;

            if (motion != nullptr) {
                const auto pos64 = motion->m_centerOfMass;
                writer->pprintf(text_pos, "%s\npos64: %f, %f, %f \nrot_physics: %f, %f, %f, %f \nphysics_ang_vel: %f, %f, %f \nphysics_vel: %f, %f, %f \ninertia: %f, %f, %f, %f \npos32: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f] \n\n",
                    actor->mIActor.name,
                    // hknpMotion
                    pos64.X, pos64.Y, pos64.Z,
                    motion->m_orientation.A, motion->m_orientation.B, motion->m_orientation.C, motion->m_orientation.D,
                    motion->m_angularVelocityLocalAndSpeedLimit.X, motion->m_angularVelocityLocalAndSpeedLimit.Y, motion->m_angularVelocityLocalAndSpeedLimit.Z,
                    motion->m_linearVelocityAndSpeedLimit.X, motion->m_linearVelocityAndSpeedLimit.Y, motion->m_linearVelocityAndSpeedLimit.Z,
                    motion->m_inverseInertia[0], motion->m_inverseInertia[1], motion->m_inverseInertia[2], motion->m_inverseInertia[3],
                    // Actor
                    actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );

            } else {
                // Actor only
                writer->pprintf(text_pos, "%s\npos32: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f]\n\n",
                    actor->mIActor.name,
                    actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );
            }
        }
    }
};


HOOK_DEFINE_INLINE(nnMainHook) {
    // Steal execution from nnMain right before it jumps into the game
    static const ptrdiff_t s_offset = sym::engine::nnMain_post_setup;
    static nnMainHookState_t main_state;
    static void Callback(exl::hook::InlineCtx* ctx) {
        // Effective entry point after sdk init

        nnMainHookState = &main_state;

        ActorRelationAddHook::Install();
        ActorRelationRemoveHook::Install();
        WorldManagerModuleBaseProcHook::Install();
        TryGetPlayerPhysicsPosPtrHook::Install();

#if DO_XXX_ACTOR_CREATION_LOG
        TestCreateActorHook1::Install();
        TestCreateActorHook2::Install();
        TestCreateActorHook3::Install();
        TestCreateActorHook4::Install();
        TestCreateActorHook5::Install();

        // WIP
        //TestCreateActorHookImplEnter::Install();
        //TestCreateActorHookImpl::Install();
        //TestDeleteActorHookImpl::Install();
#endif

        // hooks for textwriter overlay
        bool do_textwriter = (
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font.ntx") &&
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font_jis1.ntx") &&
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font_jis1_mipmap.xtx") &&
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font_jis1_tbl.bin") &&
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font_shader.bin") &&
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font_shader_jis1.bin") &&
            ConfigHelper::FileExists("content:/Lib/sead/nvn_font/nvn_font_shader_jis1_mipmap.bin") &&
            ConfigHelper::FileExists("content:/Lib/sead/primitive_renderer/primitive_drawer_nvn_shader.bin")
        );
        if (do_textwriter) {
            StealHeap::Install();
            GetCreateArg::Setup();
            GetCreateArg::Install();
            DebugDrawEnsureFont::Setup();
            DebugDrawEnsureFont::Install();
            DebugDrawImpl::Install();
        }

        // figure out where+when to connect
        char ip[16] = "";
        ConfigHelper::ReadFile(ip, "content:/totk_lotuskit/server_ip.txt", sizeof(ip), "127.0.0.1");
        bool do_connect_on_whistle = ConfigHelper::ReadFileFlag("content:/totk_lotuskit/do_connect_on_whistle.txt", true);
        bool do_connect_on_bootup = ConfigHelper::ReadFileFlag("content:/totk_lotuskit/do_connect_on_bootup.txt", true);

        if (do_connect_on_whistle) {
            LoggerConnectOnWhistleHook::Install();
        }

        // Setup RNG hooks
        // TODO hook more methods
        // TODO expose frontend setters for manip?
        HookRNG_sead_u32::do_log = ConfigHelper::ReadFileFlag("content:/totk_lotuskit/do_log_rng.txt", false);
        HookRNG_sead_u32::do_stub = ConfigHelper::ReadFileFlag("content:/totk_lotuskit/do_stub_rng.txt", false);
        HookRNG_sead_u32::stub_value = 420;
        if (HookRNG_sead_u32::do_log || HookRNG_sead_u32::do_stub) {
            // TODO move hooking decisions+actions into hook definition?
            // introspected un/re hook toggles in the frontend would be nice as a general debug thing, can leave a ton of hooks just idling for free that way...
            // also worth considering if fancy hook classes make a good backend config model
            HookRNG_sead_u32::Install();
        }

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "lotuskit using ip4 addr %s, do_connect_on_bootup: %d, do_connect_on_whistle: %d, do_textwriter: %d", ip, do_connect_on_bootup, do_connect_on_whistle, do_textwriter);
        svcOutputDebugString(buf, strlen(buf));

        // init logger socket
        auto main_logger = new Logger();
        main_state.main_logger = main_logger;
        main_logger->mState = LoggerState::UNINITIALIZED;
        main_logger->mDoOpenSocket = true; // prepare socket
        main_logger->mDoLogSocket = true; // actually send to socket
        main_logger->mDoHackCommandSocket = true; // also abuse logging socket to recv commands
        strcpy(main_logger->ip, ip);
        main_logger->init();
        if (do_connect_on_bootup) {
            main_logger->connect();
        }

        // TODO centralize on-connect info dump, or allow frontend to request it
        main_logger->logf(NS_DEFAULT_TEXT, R"("main_offset %p")", exl::util::GetMainModuleInfo().m_Total.m_Start);

        // idk spooky
        for (u8 i=0; i < sizeof(g_ModCommand_ActorWatcher); i++) { g_ModCommand_ActorWatcher[i].clear(); }

        InputHelper::initKBM();
        InputHelper::setPort(0); // default controller port
    }
};
struct nnMainHookState_t nnMainHook::main_state = {0};

