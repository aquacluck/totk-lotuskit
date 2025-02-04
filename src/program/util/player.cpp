#include "exlaunch.hpp"
#include "HexDump.hpp"
#include "TextWriter.hpp"
#include "script/globals.hpp"
#include "structs/engineActor.hpp"
#include "structs/gameComponent.hpp"
#include "util/actor.hpp"
#include "util/player.hpp"


// XXX WIP nothing in here is very useful yet

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
                lotuskit::HexDump::textwriter_printf_raw(0, dump, dump, 5);

                engine::component::ASResourceBinder* arby = &(ascomp->mASResourceBinder);
                dump = (void*)arby - 0x20;
                lotuskit::TextWriter::printf(0, "mASResourceBinder(%p) \n", arby);
                lotuskit::HexDump::textwriter_printf_raw(0, dump, dump, 5);

                as::ControllerSet* cs = (*arby).mpASControllerSet;
                dump = (void*)cs - 0x20;
                lotuskit::TextWriter::printf(0, "mpASControllerSet(%p) \n", cs);
                lotuskit::HexDump::textwriter_printf_raw(0, dump, dump, 5);

                as::Controller* cc = cs->mpCurrentController;
                dump = (void*)cc - 0x20;
                lotuskit::TextWriter::printf(0, "mpCurrentController(%p) \n", cc);
                lotuskit::HexDump::textwriter_printf_raw(0, dump, dump, 5);

                as::Blackboard** bb_ptr_member = &(cc->mpBlackboard);
                dump = (void*)bb_ptr_member - 0x10;
                lotuskit::TextWriter::printf(0, "Controller(%p) + mpBlackboard(%02x) = %p -> bb(%p) \n", cc, offsetof(as::Controller, mpBlackboard), bb_ptr_member, bb);
                lotuskit::HexDump::textwriter_printf_raw(0, dump, dump, 3);

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

    void InstallHooks() {
        //MovementDebugHooks::Install();
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
        auto pc = getPlayerComponent();
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


} // ns

