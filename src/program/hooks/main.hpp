#pragma once
#include "lib.hpp"
#include "nn/util.h"
#include "helpers/InputHelper.hpp"
#include "helpers/ConfigHelper.hpp"
#include "lib/util/sys/mem_layout.hpp"
#include "types.h"
#include "logger/Logger.hpp"
#include <string.h>
#include "sym/engine/hacks.h"

#include "hooks.hpp"
#include "hooks/main_draw.hpp"
#include "hooks/main_world.hpp"

// TODO provider hooks extract or make available some data or eventual data, with minimal side effects, except for the returned value which may be stubbed/manipulated/etc
// TODO drawer hooks may request their Draw() method to be called at agl Layer draw time, but are otherwise like provider hooks in seeking to minimize side effects.
//      base classes interfaces etc with independent namespaces, dont categorize into folders

HOOK_DEFINE_INLINE(nnMainHook) {
    // Steal execution from nnMain right before it jumps into the game
    static const ptrdiff_t s_offset = sym::engine::nnMain_post_setup; // hacks
    static void Callback(exl::hook::InlineCtx* ctx) {
        // Effective entry point after sdk init

        ActorRelationAddHook::Install();
        ActorRelationRemoveHook::Install();
        WorldManagerModuleBaseProcHook::Install(); // main_world.hpp
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
            GetCreateArg::Setup();
            GetCreateArg::Install();
            DebugDrawEnsureFont::Setup();
            DebugDrawEnsureFont::Install();
            DebugDrawImpl::Install(); // main_draw.hpp
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
        Logger::main = main_logger;
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
        for (u8 i=0; i < std::size(g_ModCommand_ActorWatcher); i++) { g_ModCommand_ActorWatcher[i].clear(); }
        for (u8 i=0; i < std::size(g_ModCommand_Hexdump); i++) { g_ModCommand_Hexdump[i].clear(); }

        InputHelper::initKBM();
        InputHelper::setPort(0); // default controller port
    }
};
