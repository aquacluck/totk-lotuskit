#pragma once

#include "const_addrs.hpp"
#include "lib.hpp"
#include "nn/hid.hpp"
#include "nn/util.hpp"
#include "helpers/InputHelper.hpp"
#include "lib/util/sys/mem_layout.hpp"
#include "structs.hpp"
#include "types.h"
#include "logger/Logger.hpp"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

// feature flags to minimize log noise and similar clutter/risk/etc
#define DO_RELATION_LOG 0
#define DO_PLAYER_LOG 1
#define DO_PHYSICS_SPAM_LOG 0
#define DO_VFR_FRAME_LOG 0
#define DO_VFR_MULTIPLIERS_LOG 0
#define DO_XXX_ACTOR_CREATION_LOG 1

uintptr_t main_offset = 0; // set in exl_main
Logger* main_logger; // set in exl_main
extern ModCommand_Savestate_ActorPos g_ModCommand_Savestate_ActorPos[5]; // XXX hooks.hpp logger.cpp trash glue

const float PLAYER_PHYSICS_HEIGHT_OFFSET = 0.9; // constant offset between pos32/pos64
ActorBase* Player = nullptr;
hknpMotion* Player_physics = nullptr;

ActorMgr* ActorMgr_instance = nullptr;
ActorBase* PlayerCamera = nullptr;
ActorBase* EventCamera = nullptr;
ActorBase* Parasail;
ActorBase* Dm_Npc_RevivalFairy;
ActorBase* CarryBox;
ActorBase* ThrowBox;



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

HOOK_DEFINE_TRAMPOLINE(LoggerConnectOnWhistleHook) {
    static const ptrdiff_t s_offset = s_ExecutePlayerWhistle_Enter;
    static void Callback(void* param) {
        main_logger->log(NS_DEFAULT_TEXT, R"("trying frontend connect() ")");
        main_logger->connect();
        main_logger->logf(NS_DEFAULT_TEXT, R"("main_offset %p")", main_offset);

        Orig(param);
    }
};

HOOK_DEFINE_TRAMPOLINE(ActorRelationAddHook) {
    static const ptrdiff_t s_offset = s_ActorMgr_addActorRelation;
    static u32 Callback(ActorMgr &actor_mgr, ActorBase &parentBase, ActorBase &childBase) {
        IActor *parent = &(parentBase.mIActor);
        IActor *child = &(childBase.mIActor);

        ActorMgr_instance = &actor_mgr;
        u8 is_skip = 0;

        if(!strcmp(parent->name, "PlayerCamera")) { PlayerCamera = &parentBase; }
        if(!strcmp(parent->name, "EventCamera")) { EventCamera = &parentBase; }

        if(!strcmp(parent->name, "Player")) {
            if(Player != &parentBase) {
                main_logger->logf(NS_DEFAULT_TEXT, R"("Reassigning Player %p -> %p")", Player, &parentBase);
                Player = &parentBase;
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
    static const ptrdiff_t s_offset = s_ActorMgr_resolveActorRelation;
    static u32 Callback(ActorMgr &actor_mgr, ActorBase &parentBase, ActorBase &childBase) {
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
    static const ptrdiff_t s_offset = s_WorldManagerModule_BaseProc;
    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
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
            Player_physics->m_centerOfMass = cmd.pos64;
            cmd.is_active_request = false;

            main_logger->logf(NS_SAVESTATE, R"({"actor_paste": ["%s"], "actor_props": [{"pos64": [%f, %f, %f], "pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] }] })",
                cmd.actor_selector_named,
                cmd.pos64.X, cmd.pos64.Y, cmd.pos64.Z,
                cmd.pos32.X, cmd.pos32.Y, cmd.pos32.Z,
                cmd.rot.m11, cmd.rot.m12, cmd.rot.m13, cmd.rot.m21, cmd.rot.m22, cmd.rot.m23, cmd.rot.m31, cmd.rot.m32, cmd.rot.m33
            );
        }


        // Logging+instrumentation in this thread comes after ^ command proc

        if (DO_PLAYER_LOG) {
            const auto& rot = Player->mRotationMatrix;
            if (Player_physics != nullptr) {
                auto pos64 = Player_physics->m_centerOfMass;
                main_logger->logf(NS_ACTOR_PLAYER, R"({"pos64": [%f, %f, %f], "rot_physics": [%f, %f, %f, %f], "physics_ang_vel": [%f, %f, %f], "physics_vel": [%f, %f, %f], "inertia": [%f, %f, %f, %f], "pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] })",
                    // hknpMotion
                    pos64.X, pos64.Y, pos64.Z,
                    Player_physics->m_orientation.A, Player_physics->m_orientation.B, Player_physics->m_orientation.C, Player_physics->m_orientation.D,
                    Player_physics->m_angularVelocityLocalAndSpeedLimit.X, Player_physics->m_angularVelocityLocalAndSpeedLimit.Y, Player_physics->m_angularVelocityLocalAndSpeedLimit.Z,
                    Player_physics->m_linearVelocityAndSpeedLimit.X, Player_physics->m_linearVelocityAndSpeedLimit.Y, Player_physics->m_linearVelocityAndSpeedLimit.Z,
                    Player_physics->m_inverseInertia[0], Player_physics->m_inverseInertia[1], Player_physics->m_inverseInertia[2], Player_physics->m_inverseInertia[3],
                    // Actor
                    Player->mPosition.X, Player->mPosition.Y, Player->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );

                // TODO try writing angles, velocities, prev+now, inertia, ... just so we know what happens
                // TODO try blanking out / early returning / etc selectively to see how much we can fix movement... needs actor<->phive<->hknpBody/Shape<->hknpMotion / similar association?
            } else {

                // Actor only
                main_logger->logf(NS_ACTOR_PLAYER, R"({"pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] })",
                    Player->mPosition.X, Player->mPosition.Y, Player->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );
            }
        }

        // "raw" time is affected by bullet time and not much else (not ascend, recall, textboxes, textbox logs, pausing, memories)
        // bt drops to 0.05 0.001667, then 4 interpolated frames back up to normal 1.0 0.033333 when it ends. This can stack/multiply with perf hits:
        // Both raw and cooked are affected by performance:
        // On 1.2.1 it will switch to 1.5 0.05 during heavy load. That's 1.5x the normal duration, or 0.05 = 1/20 seconds.
        // 1.0 all seems the same. I thought only 1.0 was locked between 20fps and 30fps?
        VFRMgr** vfr_mgr = (VFRMgr**)(main_offset + s_VFRMgr_sInstance);
        //main_logger->log("vfr_mgr %p -> %p", vfr_mgr, *vfr_mgr)
        if (DO_VFR_FRAME_LOG) {
            main_logger->logf(NS_VFRMGR, R"({"vfr_frame": [%f, %f, %f, %f, %f] })", (*vfr_mgr)->mRawDeltaFrame, (*vfr_mgr)->mRawDeltaTime, (*vfr_mgr)->mDeltaFrame, (*vfr_mgr)->mDeltaTime, (*vfr_mgr)->mIntervalValue);
        }
        if (DO_VFR_MULTIPLIERS_LOG) {
            for(int i = 0; i < 16; i++) {
                // default all slots: 1.0 nan
                // during bt: 0 -> 0.05 0.05, 3 -> 0.35 0.35
                main_logger->logf(NS_VFRMGR, R"({"vfr_timespeedmultiplier_i": %d, "pair": [%f, %f] })", i, (*vfr_mgr)->mTimeSpeedMultipliers[i].mValue, (*vfr_mgr)->mTimeSpeedMultipliers[i].mTarget);
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
    static const ptrdiff_t s_offset = s_tryGetPlayerPhysicsPosPtrHook;
    static void Callback(void* self, void* param_2, hknpMotion* hkmotion, hknpMotion* motion_pool) {

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
        }

        Orig(self, param_2, hkmotion, motion_pool);
    }
};


#if DO_XXX_ACTOR_CREATION_LOG
HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook1) {
    static const ptrdiff_t s_offset = s_xxx_createActor1;
    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,undefined **param_8,undefined *param_9,
        undefined8 param_10,void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,
        undefined8 *param_14,undefined4 param_16,undefined4 param_17,undefined8 param_15, undefined param_19) {

        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_sync1"})", *param_4); // only Obj_OneTouch_Connection, no overlap

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook2) {
    static const ptrdiff_t s_offset = s_xxx_createActor2;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,undefined **param_8,undefined *param_9,
        undefined8 param_10,void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,
        undefined8 *param_14,undefined4 param_16,undefined4 param_17,undefined8 param_15, undefined param_19) {

        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_idk2"})", *param_4); // XXX never hit?

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook3) {
    static const ptrdiff_t s_offset = s_xxx_createActor3;
    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,long param_8,long param_9,undefined8 param_10,
        void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,long *param_14,
        undefined4 param_16,undefined4 param_17,undefined8 param_15,undefined param_19) {

        // only relation names (eg fused rocks, fused wings, but weird stuff too like quickmenu-icon-screenshotting and pause-actor instantiations). no overlap with other hooks
        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_relation"})", *param_4);

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook4) {
    static const ptrdiff_t s_offset = s_xxx_createActor4;
    static long Callback
        (void* actorInstanceMgr, char** actorName, Vector3f* pos, void* param_4, undefined4 param_5, PreActor* preActor,
        void* actorObserver, void* param_8, u8 param_9, undefined4* param_10, PreActor** destPreActor) {

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

        return ret;
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook5) {
    static const ptrdiff_t s_offset = s_xxx_createActor5;

    static long Callback(long **param_1,char **param_2,long param_3,long param_4,long param_5,undefined8 param_6, int *param_7) {
        // few actors at bootup: ReactionHit ReactionField Chemical EffectUI SpecialPower WakeBoardRope
        main_logger->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_sync"})", *param_2);
        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7);
    }
};
#endif
