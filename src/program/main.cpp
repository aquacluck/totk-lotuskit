#include "lib.hpp"
#include "nn/util.h"

#include "Config.hpp"
#include "server/WebSocket.hpp"
#include "script/engine.hpp"
#include "script/globals.hpp"

#include "sym/engine/hacks.h"
#include "sym/game/wm/WorldManagerModule.h"

#include "angelscript.h"
#include "heap/seadHeap.h"


HOOK_DEFINE_INLINE(StealHeap) {
    static const ptrdiff_t s_offset = sym::engine::steal_heap; // hacks
    inline static sead::Heap* stolenHeap = nullptr;
    static void Callback(exl::hook::InlineCtx* ctx) {
#ifdef TOTK_100
        // TODO register annotation/aliasing instead
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[19]);
#else
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[22]);
#endif
        svcOutputDebugString("yoink", 5);
        lotuskit::script::engine::AssignHeap(stolenHeap);
    }
};

void TestAngelScript() {
    // TODO extract a longer lived engine instance, route ws+tas through it
    //auto* engine = lotuskit::script::engine::CreateAndSetupEngine();
    AngelScript::asIScriptEngine *engine = AngelScript::asCreateScriptEngine();
    lotuskit::script::engine::ConfigureEngine(engine);
    lotuskit::script::globals::RegisterGlobals(engine);

    const char* scriptText = R"( void main() { int wow = 419; TrashPrintInt(++wow); } )";
    const char* entryPoint = "void main()";
    auto* mod = lotuskit::script::engine::TestBuildModule(engine, scriptText);
    lotuskit::script::engine::TestExecFuncInNewCtx(engine, mod, entryPoint);

    engine->ShutDownAndRelease();
}

/*
#include "sym/game/ai/execute/ExecutePlayerWhistle.h"
HOOK_DEFINE_TRAMPOLINE(OnWhistleHook) {
    static const ptrdiff_t s_offset = sym::game::ai::execute::ExecutePlayerWhistle::enterImpl_;
    static void Callback(void* param) {
        TestAngelScript();
        Orig(param);
    }
};
*/

HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static const auto s_offset = sym::game::wm::WorldManagerModule::baseProcExe;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        TestAngelScript();

        if (false) { // make noise
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
        }

        lotuskit::server::WebSocket::Calc(); // noblock recv, but blocking processing if enabled

        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};

HOOK_DEFINE_INLINE(nnMainHook) {
    static const ptrdiff_t s_offset = sym::engine::nnMain_post_setup; // hacks
    static void Callback(exl::hook::InlineCtx* ctx) {
        // Effective entry point after sdk init
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] nnMainHook main_offset=%p", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        svcOutputDebugString(buf, strlen(buf));

        lotuskit::Config::LoadJson();
        auto& config = lotuskit::Config::jsonConfig;

        const json::json_pointer GLOBAL_DISABLE("/global/disable");
        if (config.contains(GLOBAL_DISABLE) && config[GLOBAL_DISABLE]) {
            nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] all features disabled", TOTK_VERSION);
            svcOutputDebugString(buf, strlen(buf));
            return;
        }

        lotuskit::server::WebSocket::Calc(); // blocking if listenOnBootup
        WorldManagerModuleBaseProcHook::Install(); // "main loop"
        StealHeap::Install(); // called once, a bit later during bootup
        //OnWhistleHook::Install();


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
