#include "lib.hpp"
#include "nn/util.h"

#include "config.hpp"
#include "server/WebSocket.hpp"

#include "sym/engine/hacks.h"
#include "sym/game/wm/WorldManagerModule.h"


HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static const auto s_offset = sym::game::wm::WorldManagerModule::baseProcExe;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        static u64 lastPrintTick = 0;
        u64 thisTick = svcGetSystemTick();
        if (thisTick >= lastPrintTick + 20000000) {
            lastPrintTick = thisTick;

            char buf[200];
            nn::util::SNPrintf(buf, sizeof(buf), R"({ "wmprocTick": "0x%p", "doSend": true })", svcGetSystemTick());
            svcOutputDebugString(buf, strlen(buf));

            lotuskit::server::WebSocket::SendTextNoblock(buf);
            lotuskit::server::WebSocket::SendTextNoblock(buf);
            //lotuskit::server::WebSocket::SendTextBlocking(buf);
        }

        lotuskit::server::WebSocket::RecvNoblockAndProc(); // noblock poll ws -> dispatch commands (sometimes blocking)
        lotuskit::server::WebSocket::FlushSendQueueBlocking(); // send any deferred logs since last proc. try to minimize useless logs!
        // FIXME until we get threaded sends to work, logging may impact performance/accuracy/???
        // TODO run some tests to verify ^ this is a real issue, use devtools throttling to break it, spam it etc and see what really blocks

        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};

HOOK_DEFINE_INLINE(nnMainHook) {
    static const ptrdiff_t s_offset = sym::engine::nnMain_post_setup; // hacks
    static void Callback(exl::hook::InlineCtx* ctx) {
        // Effective entry point after sdk init, stealing execution from nnMain right before it jumps into the game
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] nnMainHook main_offset=%p", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        svcOutputDebugString(buf, strlen(buf));

        lotuskit::config::LoadJson();
        auto& config = lotuskit::config::jsonConfig;

        const json::json_pointer GLOBAL_DISABLE("/global/disable");
        if (config.contains(GLOBAL_DISABLE) && config[GLOBAL_DISABLE]) {
            nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] all features disabled", TOTK_VERSION);
            svcOutputDebugString(buf, strlen(buf));
            return;
        }

        lotuskit::server::WebSocket::CreateAndWaitForFrontend(); // blocking if enabled

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
