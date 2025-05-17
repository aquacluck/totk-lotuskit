#include "exlaunch.hpp"
#include <nn/hid.h>
#include "ActorWatcher.hpp"
#include "HexDump.hpp"
#include "TextWriter.hpp"
#include "script/globals.hpp"
#include "structs/engineActor.hpp"
#include "structs/gameComponent.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"
#include "util/actor.hpp"
#include "util/player.hpp"


namespace lotuskit::util::player {

    namespace MovementDebugHooks {
        /*
        HOOK_DEFINE_TRAMPOLINE(PlayerComponentCheckContactMiasmaHook) {
            static constexpr auto s_name = "game::component::PlayerComponent::checkContactMiasma";
            inline static bool do_disable = false;
            static u64 Callback(void* pc, sead::Vector3f* idkout, u8* out) {
                if (!do_disable) { return Orig(pc, idkout, out); }
                *out = 0; // FIXME dont work :(
                return 0;
            }
        };
        void disableGloom(bool do_disable) {
            PlayerComponentCheckContactMiasmaHook::do_disable = do_disable;
        }

        HOOK_DEFINE_TRAMPOLINE(PlayerComponentSetForwardDirAndSpeedHook) {
            //static constexpr auto s_name = "game::component::PlayerComponent::setForwardDirAndSpeed";
            static void Callback(float speed, void* pc, sead::Vector3f* dir) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "PlayerComponent::setForwardDirAndSpeed(%f, %p, {%f, %f, %f})", speed, pc, dir->x, dir->y, dir->z);
                svcOutputDebugString(buf, strlen(buf));
                return Orig(speed, pc, dir);
            }
        };

        HOOK_DEFINE_TRAMPOLINE(PlayerComponentSetForwardSpeedAndAngularVelHook) {
            //static constexpr auto s_name = "game::component::PlayerComponent::setForwardSpeedAndAngularVel";
            static void Callback(float speed, void* pc, sead::Vector3f* dir) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "PlayerComponent::setForwardSpeedAndAngularVel(%f, %p, {%f, %f, %f})", speed, pc, dir->x, dir->y, dir->z);
                svcOutputDebugString(buf, strlen(buf));
                return Orig(speed, pc, dir);
            }
        };

        HOOK_DEFINE_TRAMPOLINE(PlayerComponentSetForwardSpeedHook) {
            //static constexpr auto s_name = "game::component::PlayerComponent::setForwardSpeed";
            static void Callback(float speed, void* pc) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "PlayerComponent::setForwardSpeed(%f, %p)", speed, pc);
                svcOutputDebugString(buf, strlen(buf));
                return Orig(speed, pc);
            }
        };

        HOOK_DEFINE_TRAMPOLINE(PlayerComponentSetForwardDirHook) {
            //static constexpr auto s_name = "game::component::PlayerComponent::setForwardDir";
            static void Callback(void* pc, sead::Vector3f* dir) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "PlayerComponent::setForwardDir(%p, {%f, %f, %f})", pc, dir->x, dir->y, dir->z);
                svcOutputDebugString(buf, strlen(buf));
                return Orig(pc, dir);
            }
        };

        HOOK_DEFINE_TRAMPOLINE(GameActorUpdatePlayerForwardDirAndSpeed) {
            //static constexpr auto s_name = "game::component::PlayerComponent::setForwardDir";
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before GameActor::updatePlayerForwardDirAndSpeed(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                nn::util::SNPrintf(buf, sizeof(buf), "after GameActor::updatePlayerForwardDirAndSpeed(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };

        HOOK_DEFINE_TRAMPOLINE(movehook) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };

        HOOK_DEFINE_TRAMPOLINE(movehook_sub1) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_::sub1(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_::sub1(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };

        HOOK_DEFINE_TRAMPOLINE(movehook_sub2) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_::sub2(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_::sub2(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };

        HOOK_DEFINE_TRAMPOLINE(movehook_sub3) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_::sub3(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_::sub3(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };

        HOOK_DEFINE_TRAMPOLINE(movehook_sub4a) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_::sub4a(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_::sub4a(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };
        HOOK_DEFINE_TRAMPOLINE(movehook_sub4b) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_::sub4b(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_::sub4b(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };
        HOOK_DEFINE_TRAMPOLINE(movehook_sub4c) {
            static void Callback(void* a) {
                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "before ExecutePlayerMove::updateImpl_::sub4c(%p)", a);
                svcOutputDebugString(buf, strlen(buf));

                Orig(a);

                //char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "after ExecutePlayerMove::updateImpl_::sub4c(%p)", a);
                svcOutputDebugString(buf, strlen(buf));
            }
        };

        */
        HOOK_DEFINE_TRAMPOLINE(movehook_sub_scaleForBBPerimeterGrade) {
            static float Callback(float groundedXYZVelScale, engine::actor::ActorBase* player) {
                // fetches PlayerFloatASKey::PerimeterGrade bb float, adjusts(?) and scales input vel
                float outForwardVel = Orig(groundedXYZVelScale, player);

                lotuskit::util::actor::dumpASBlackboard(player);

                /*
                auto* bb = player->getASComponent()->mASResourceBinder.mpASControllerSet->mpCurrentController->mpBlackboard;
                float pg = bb->getFloatByKey("PerimeterGrade");
                //void* bb_vtbl_offset = nullptr; // *((void**)bb) - (exl::util::GetMainModuleInfo().m_Total.m_Start);

                char buf[400];
                nn::util::SNPrintf(buf, sizeof(buf), "ExecutePlayerMove::updateImpl_::sub_scaleForBBPerimeterGrade(%f, %p) -> %f", groundedXYZVelScale, player, outForwardVel);
                svcOutputDebugString(buf, strlen(buf));

                lotuskit::TextWriter::printf(0, "main_offset(%p) \n", exl::util::GetMainModuleInfo().m_Total.m_Start);
                engine::component::ASComponent* ascomp = player->getASComponent();
                void* dump = (void*)ascomp - 0x20;
                lotuskit::TextWriter::printf(0, "ASComponent(%p) \n", ascomp);
                lotuskit::HexDump::textwriter_printf_u8(0, dump, dump, 5);

                engine::component::ASResourceBinder* arby = &(ascomp->mASResourceBinder);
                dump = (void*)arby - 0x20;
                lotuskit::TextWriter::printf(0, "mASResourceBinder(%p) \n", arby);
                lotuskit::HexDump::textwriter_printf_u8(0, dump, dump, 5);

                as::ControllerSet* cs = (*arby).mpASControllerSet;
                dump = (void*)cs - 0x20;
                lotuskit::TextWriter::printf(0, "mpASControllerSet(%p) \n", cs);
                lotuskit::HexDump::textwriter_printf_u8(0, dump, dump, 5);

                as::Controller* cc = cs->mpCurrentController;
                dump = (void*)cc - 0x20;
                lotuskit::TextWriter::printf(0, "mpCurrentController(%p) \n", cc);
                lotuskit::HexDump::textwriter_printf_u8(0, dump, dump, 5);

                as::Blackboard** bb_ptr_member = &(cc->mpBlackboard);
                dump = (void*)bb_ptr_member - 0x10;
                lotuskit::TextWriter::printf(0, "Controller(%p) + mpBlackboard(%02x) = %p -> bb(%p) \n", cc, offsetof(as::Controller, mpBlackboard), bb_ptr_member, bb);
                lotuskit::HexDump::textwriter_printf_u8(0, dump, dump, 3);

                nn::util::SNPrintf(buf, sizeof(buf), "ExecutePlayerMove::updateImpl_::sub_scaleForBBPerimeterGrade: player(%p) bb(%p) pg(%f)", player, bb, pg);
                //nn::util::SNPrintf(buf, sizeof(buf), "ExecutePlayerMove::updateImpl_::sub_scaleForBBPerimeterGrade: bb(%p vtbl %p) ", bb, bb_vtbl_offset);
                svcOutputDebugString(buf, strlen(buf));
                */

                return outForwardVel;
            }
        };

        /*
        HOOK_DEFINE_TRAMPOLINE(PlayerComponentGetAdjustedRetargetPosAndRot) {
            static u64 Callback(void* pc, sead::Vector3f* pos, sead::Vector3f* rot) {
                u64 ret = Orig(pc, pos, rot);

                char buf[400];
                if (pos == nullptr || rot == nullptr) {
                    nn::util::SNPrintf(buf, sizeof(buf), "PlayerComponent::getAdjustedRetargetPosAndRot(%p, out{%p}, out{%p}) -> %p", pc, pos, rot, ret);
                } else {
                    nn::util::SNPrintf(buf, sizeof(buf), "PlayerComponent::getAdjustedRetargetPosAndRot(%p, out{%f, %f, %f}, out{%f, %f, %f}) -> %p", pc, pos->x, pos->y, pos->z, rot->x, rot->y, rot->z, ret);
                }
                svcOutputDebugString(buf, strlen(buf));

                return ret;
            }
        };
        */

        void Install() {
#ifdef TOTK_121
            //PlayerComponentCheckContactMiasmaHook::Install();
            //PlayerComponentSetForwardDirAndSpeedHook::InstallAtOffset(0x00bf8ba0); // 121
            //PlayerComponentSetForwardSpeedAndAngularVelHook::InstallAtOffset(0x016218c0); // 121
            //PlayerComponentSetForwardSpeedHook::InstallAtOffset(0x00bf8b48); // 121
            //PlayerComponentSetForwardDirHook::InstallAtOffset(0x01621c64); // 121
            //GameActorUpdatePlayerForwardDirAndSpeed::InstallAtOffset(0x00bf8aa0); // 121
            //movehook::InstallAtOffset(0x01d6aa6c); // 121
            //movehook_sub1::InstallAtOffset(0x01d6ab1c); // 121
            //movehook_sub2::InstallAtOffset(0x01d6ac50); // 121
            //movehook_sub3::InstallAtOffset(0x01d6ac50); // 121
            // updateinput?, updateanim?
            //movehook_sub4a::InstallAtOffset(0x01d6b728); // 121
            //movehook_sub4b::InstallAtOffset(0x01d6b558); // 121
            //movehook_sub4c::InstallAtOffset(0x01d6b3d8); // 121
            movehook_sub_scaleForBBPerimeterGrade::InstallAtOffset(0x01d6bde0); // 121
            //PlayerComponentGetAdjustedRetargetPosAndRot::InstallAtOffset(0x00bf8478); // 121
#endif
        }
    } // ns

    HOOK_DEFINE_INLINE(ExecutePlayerCalcStick_CameraHook) {
        static constexpr auto s_name = "game::ai::execute::ExecutePlayerCalcStick::updateImpl_::HACK_Camera";
        static void Callback(exl::hook::InlineFloatCtx* ctx) {
            constexpr auto sx = 11; // for 100, 110, 121
            constexpr auto sz = 8; // for 100, 110, 121

            // dump
            doLStickAbsoluteCameraFreezeXDump = ctx->S[sx];
            doLStickAbsoluteCameraFreezeZDump = ctx->S[sz];
            if (doLStickAbsoluteMode == 4) {
                // freeze
                ctx->S[sx] = doLStickAbsoluteCameraFreezeX;
                ctx->S[sz] = doLStickAbsoluteCameraFreezeZ;
            }
        }
    };

    HOOK_DEFINE_INLINE(ExecutePlayerCalcStick_WriteLeftStickRadianCameraStandardHook) {
        static constexpr auto s_name = "game::ai::execute::ExecutePlayerCalcStick::updateImpl_::HACK_WriteLeftStickRadianCameraStandard";
        static void Callback(exl::hook::InlineFloatCtx* ctx) {
            if (doLStickAbsoluteMode == 0) { return; } // vanilla/nop
            if (doLStickAbsoluteMode == 4) { return; } // nop here, done in CameraHook
            constexpr auto si = TOTK_VERSION == 121 ? 2 : 15; // for 100, 110, 121

            /// XXX copypasta from InputDisplay: get effective LStick
            // copy human input: contains latest polled input even when not recording
            nn::hid::NpadBaseState input = {0};
            std::memcpy((void*)&(input.mButtons), (void*)&(lotuskit::tas::Record::currentInput.buttons), 24);
            // alter/override/passthrough according to current options
            lotuskit::tas::Playback::applyCurrentInput(&input);
            s32 LX = input.mAnalogStickL.mX;
            s32 LY = input.mAnalogStickL.mY;
            /// XXX end tasty pasta
            const float lrad = atan2(-LX, LY);

            if (doLStickAbsoluteMode == 1) {
                //lotuskit::TextWriter::printf(0, "lrad %f, doLStickAbsoluteRadOffset %f\n", lrad, doLStickAbsoluteRadOffset);
                ctx->S[si] = lrad + doLStickAbsoluteRadOffset;

            } else if (doLStickAbsoluteMode == 2) {
                auto player = lotuskit::script::globals::ResidentActors::Player;
                float dx = doLStickAbsoluteTargetPos.x - player->mPosition.x;
                float dz = -(doLStickAbsoluteTargetPos.z - player->mPosition.z);
                ctx->S[si] = lrad + atan2(-dx, dz) + M_PI;

            } else if (doLStickAbsoluteMode == 3) {
                auto player = lotuskit::script::globals::ResidentActors::Player;
                auto target = lotuskit::ActorWatcher::getSlotActor(doLStickAbsoluteTargetActorWatcher);
                if (target == nullptr) { return; } // XXX warn?
                float dx = target->mPosition.x - player->mPosition.x;
                float dz = -(target->mPosition.z - player->mPosition.z);
                ctx->S[si] = lrad + atan2(-dx, dz) + M_PI;

            } else if (doLStickAbsoluteMode == 5) {
                auto player = lotuskit::script::globals::ResidentActors::Player;
                sead::Vector3f rot = {0, 0, 0};
                player->mRotation.makeR(rot);
                ctx->S[si] = lrad + rot.y;
            }
        }
    };

    HOOK_DEFINE_TRAMPOLINE(StaminaCalculatorCalcRecoveryHook) {
        static constexpr auto s_name = "game::player::StaminaCalculator::calcRecovery";
        static void Callback(game::player::StaminaCalculator* self, void* idk1, u32 idk2_maybe_receiver_signature_flag) {
            if (doTextWriterStaminaCalculator) {
                lotuskit::TextWriter::printf(0, "StaminaCalculator(%p), stamina(%f), freeze(%f), flags(%02x), amount(%f), time(%f)\n",
                    self, self->mStamina, freezeStaminaRecoveryAmount, self->mFlags, self->mStaminaRecoverAmount, self->mNoRecoveryTimer.mTime);
            }

            if (freezeStaminaExhaustionMode) {
                self->mStaminaRecoverAmount = 0;
                self->mFlags = 1; // always exhausted?
                self->mNoRecoveryTimer.mTime = 2; // never recover

            } else if (freezeStaminaRecoveryAmount > 0) {
                self->mStaminaRecoverAmount = freezeStaminaRecoveryAmount;
                self->mFlags = 0; // never exhausted?
                self->mNoRecoveryTimer.mTime = 0; // always recover asap (recovery can still be blocked by climbing+idk)

            } else if (freezeStaminaRecoveryAmount < 0) {
                self->mStaminaRecoverAmount = 0;
                // nop wrt exhaustion
                self->mNoRecoveryTimer.mTime = 2; // never recover
            }

            Orig(self, idk1, idk2_maybe_receiver_signature_flag);
        }
    };

    void InstallHooks() {
        ExecutePlayerCalcStick_CameraHook::Install();
        ExecutePlayerCalcStick_WriteLeftStickRadianCameraStandardHook::Install();
        //MovementDebugHooks::Install();
        StaminaCalculatorCalcRecoveryHook::Install();
    }

    game::component::PlayerComponent* getPlayerComponent() {
        // FIXME cast/store Player as GameActor not ActorBase?
        auto player = lotuskit::script::globals::ResidentActors::Player;
        if (player == nullptr) { return nullptr; }
        return player->getPlayerComponent();
    }

    /*
    // XXX same problem as doing this on Player's RigidBody -- doesn't reliably apply/hold
    void setLinearVelocity(sead::Vector3f* vel) {
        auto* pc = getPlayerComponent();
        if (pc == nullptr) { return; }
        u64 doScale30 = 0;

        using impl_t = void (game::component::PlayerComponent*, sead::Vector3f*, u64);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("game::component::PlayerComponent::setLinearVelocity");
        return impl(pc, vel, doScale30);
    }
    void setLinearVelocityXYZ(float x, float y, float z) {
        auto vel = sead::Vector3f{x, y, z};
        setLinearVelocity(&vel);
    }
    */

    float getStamina() {
        auto* pc = getPlayerComponent();
        if (pc == nullptr) { return 0; }
        return pc->mStaminaCalculator.mStamina;
    }
    void setStamina(float stam) {
        auto* pc = getPlayerComponent();
        if (pc == nullptr) { return; }
        pc->mStaminaCalculator.mStamina = stam;
        pc->mStaminaCalculator.mStaminaInternal = stam; // XXX wut do
    }

} // ns

