#include "lib.hpp"
#include "nn/util.h"

#include "config.hpp"
#include "server/SocketThread.hpp"

#include "sym/engine/hacks.h"
#include "sym/game/wm/WorldManagerModule.h"


HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static const auto s_offset = sym::game::wm::WorldManagerModule::baseProcExe;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};

HOOK_DEFINE_INLINE(nnMainHook) {
    // Steal execution from nnMain right before it jumps into the game
    static const ptrdiff_t s_offset = sym::engine::nnMain_post_setup; // hacks
    static void Callback(exl::hook::InlineCtx* ctx) {
        // Effective entry point after sdk init
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] nnMainHook main_offset=%p", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        svcOutputDebugString(buf, strlen(buf));

        lotuskit::config::LoadJson();
        auto& config = lotuskit::config::json_config;

        if (config.contains("disable") && config["disable"].template get<bool>()) {
            nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] all features disabled");
            svcOutputDebugString(buf, strlen(buf));
            return;
        }

        if (config["start_server_on_bootup"].template get<bool>()) {
            // XXX make sure starting this later will work -- config name shouldnt imply things we cant actually do
            lotuskit::server::SocketThread::CreateAndWaitForFrontend(); // block
        }

        WorldManagerModuleBaseProcHook::Install();

        /*
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
            DebugDrawImpl::Install(); // main_draw.hpp
        }

        if (do_connect_on_whistle) {
            LoggerConnectOnWhistleHook::Install();
        }

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
        */

        //InputHelper::initKBM();
        //InputHelper::setPort(0); // default controller port
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    // if you launch the wrong version this is all you'll see
    char buf[] = "[totk-lotuskit:___] exl_main";
    buf[15] = '0' + TOTK_VERSION/100;
    buf[16] = '0' + TOTK_VERSION%100/10;
    buf[17] = '0' + TOTK_VERSION%10;
    svcOutputDebugString(buf, strlen(buf));

    exl::hook::Initialize();
    nnMainHook::Install();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
