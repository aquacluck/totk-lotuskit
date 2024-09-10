#pragma once
#include "helpers/InputHelper.hpp"
#include "lib.hpp"
#include "lib/util/ptr_path.hpp"
#include "logger/Logger.hpp"

#include "sym/engine/module/VFRMgr.h"
#include "sym/game/wm/WorldManagerModule.h"

extern ModCommand_ActorWatcher g_ModCommand_ActorWatcher[4]; // XXX trash glue
extern ModCommand_Savestate_ActorPos g_ModCommand_Savestate_ActorPos[4]; // XXX trash glue

HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static const auto s_offset = sym::game::wm::WorldManagerModule::baseProcExe;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        InputHelper::updatePadState();

        ActorBase* Player = g_ModCommand_ActorWatcher[0].actor;
        if (Player == nullptr) {
            // early return to wait for the first relation to populate Player. sloppy but it's just bootup, not much going on anyways
            return Orig(self, param_2, param_3, param_4, wmmodule, param_6);
        }

        // Parse any pending commands on the wire
        Logger::main->tryRecvCommandMainLoop(); // XXX filthy hack, nonblocking recv

        // Command: actor paste
        // TODO efficiently know when+where work items are pending in this arr
        auto &cmd = g_ModCommand_Savestate_ActorPos[0];
        if(cmd.is_active_request && strcmp("Player", cmd.actor_selector_named) == 0) {
            // FIXME rotation doesn't restore, it only affects the map.
            // FIXME model often doesn't warp until movement, similar to freezing coords

            Player->mPosition = cmd.pos32;
            Player->mRotationMatrix = cmd.rot;

            hknpMotion* Player_physics = g_ModCommand_ActorWatcher[0].tracked_motion;
            Player_physics->m_centerOfMass.X = cmd.pos64.X;
            Player_physics->m_centerOfMass.Y = cmd.pos64.Y;
            Player_physics->m_centerOfMass.Z = cmd.pos64.Z;
            //Player_physics->m_centerOfMass = cmd.pos64; // FIXME spooky assignments, just copy members
            cmd.is_active_request = false;

            Logger::main->logf(NS_SAVESTATE, R"({"actor_paste": ["%s"], "actor_props": [{"pos64": [%f, %f, %f], "pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] }] })",
                cmd.actor_selector_named,
                cmd.pos64.X, cmd.pos64.Y, cmd.pos64.Z,
                cmd.pos32.X, cmd.pos32.Y, cmd.pos32.Z,
                cmd.rot.m11, cmd.rot.m12, cmd.rot.m13, cmd.rot.m21, cmd.rot.m22, cmd.rot.m23, cmd.rot.m31, cmd.rot.m32, cmd.rot.m33
            );
        }

        // Resolve pending actor selection via preactors
        // TODO efficiently pick slots needing resolve work
        // TODO check for entries that take too long to resolve -> hack to avoid checking this every frame for those cases (or just finish the create actor hooking and stop polling)
        for (u8 i=1; i < std::size(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_pending_selection || cmd.selection_type == 0 || cmd.preactor == nullptr) { continue; }
            if (cmd.preactor->mpActor != nullptr) {
                // FIXME this traversal is broken on 1.0.0? bad actor pointer
                // eg Logger::main->logf(NS_DEFAULT_TEXT, R"({"player preactor": "%p -> %p"})", Player_PreActor, Player_PreActor->mpActor);
                cmd.actor = cmd.preactor->mpActor;
                cmd.is_pending_selection = false;
                cmd.is_publishing_selection = true;
            }
        }

        // Logging+instrumentation in this thread comes after ^ command proc

        for (u8 i=0; i < std::size(g_ModCommand_Hexdump); i++) {
            auto &cmd = g_ModCommand_Hexdump[i];
            if (!cmd.is_calc || cmd.dump_src == nullptr) { cmd.calc_age++; continue; }

            // dump into buffer
            auto len = abs(cmd.dump_len);
            len = len > ModCommand_Hexdump::BUF_LEN ? ModCommand_Hexdump::BUF_LEN : len;
            std::memcpy(cmd.buf, cmd.dump_src, len);

            cmd.calc_age = 0;
            if (cmd.is_pending_draw) {
                cmd.is_pending_draw = false;
                cmd.is_draw = true;
            }
        }

        for (u8 i=0; i < std::size(g_ModCommand_ActorWatcher); i++) {
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
                Logger::main->logf(ns, R"({"pos64": [%f, %f, %f], "rot_physics": [%f, %f, %f, %f], "physics_ang_vel": [%f, %f, %f], "physics_vel": [%f, %f, %f], "inertia": [%f, %f, %f, %f], "pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] })",
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
                Logger::main->logf(ns, R"({"pos32": [%f, %f, %f], "rot": [%f, %f, %f, %f, %f, %f, %f, %f, %f] })",
                    actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                    rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                );
            }
        }

        constexpr bool DO_VFR_LOG = 0; // multipliers especially are pretty spammy TODO extract timing info to a provider, flag for logs
        if (DO_VFR_LOG) {
            // "raw" time is affected by bullet time and not much else (not ascend, recall, textboxes, textbox logs, pausing, memories)
            // bt drops to 0.05 0.001667, then 4 interpolated frames back up to normal 1.0 0.033333 when it ends. This can stack/multiply with perf hits:
            // Both raw and cooked are affected by performance:
            // On 1.2.1 it will switch to 1.5 0.05 during heavy load. That's 1.5x the normal duration, or 0.05 = 1/20 seconds.
            // 1.0 all seems the same. I thought only 1.0 was locked between 20fps and 30fps?
            VFRMgr* vfr_mgr = *exl::util::pointer_path::FollowSafe<VFRMgr*, sym::engine::module::VFRMgr::sInstance>();
            Logger::main->logf(NS_VFRMGR, R"({"vfr_frame": [%f, %f, %f, %f, %f] })", vfr_mgr->mRawDeltaFrame, vfr_mgr->mRawDeltaTime, vfr_mgr->mDeltaFrame, vfr_mgr->mDeltaTime, vfr_mgr->mIntervalValue);
            for(int i = 0; i < 16; i++) {
                // default all slots: 1.0 nan
                // during bt: 0 -> 0.05 0.05, 3 -> 0.35 0.35
                Logger::main->logf(NS_VFRMGR, R"({"vfr_timespeedmultiplier_i": %d, "pair": [%f, %f] })", i, vfr_mgr->mTimeSpeedMultipliers[i].mValue, vfr_mgr->mTimeSpeedMultipliers[i].mTarget);
            }
        }


        if (InputHelper::isKeyPress(nn::hid::KeyboardKey::Backquote)) {
            Logger::main->log(NS_DEFAULT_TEXT, "\"oink\"");
            // XXX HACK bring up mr dumpo, then toggle calc
            auto &dumpo = g_ModCommand_Hexdump[2];
            if (!dumpo.is_calc) {
                void* ptr = (void*)(g_ModCommand_ActorWatcher[0].tracked_motion);
                if (ptr != nullptr) {
                    dumpo.is_calc = true;
                    dumpo.is_pending_draw = true;
                    strcpy(dumpo.label, "Player main hknpMotion");
                    dumpo.dump_src = ptr;
                    dumpo.dump_len = 0x80;
                    dumpo.draw_len = 0x80;
                }
            } else {
                dumpo.is_calc = false; // leave the stale dump up to draw
            }
        }


        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};
