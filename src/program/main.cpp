#include "lib.hpp"
#include "nn/util.h"

#include "config.hpp"
#include "server/WebSocket.hpp"

#include "sym/engine/hacks.h"
#include "sym/game/ai/execute/ExecutePlayerWhistle.h"
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
    }
};

inline void* ASAlloc(size_t size) { return StealHeap::stolenHeap->alloc(size); }
inline void ASFree(void* ptr) { StealHeap::stolenHeap->free(ptr); }

//RegisterStdString(engine); // TODO simpler string impl?
//void ASTrashPrint(auto &msg) {
//    const char* str = msg.c_str();
//    svcOutputDebugString(str, strlen(str));
//}
void ASTrashPrintInt(int num) {
    char buf[1000];
    nn::util::SNPrintf(buf, sizeof(buf), "oink %d", num);
    svcOutputDebugString(buf, strlen(buf));
}

void ASMessageCallback(const asSMessageInfo *msg, void *param) {
    const char *type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING) {
        type = "WARN ";
    } else if(msg->type == asMSGTYPE_INFORMATION) {
        type = "INFO ";
    }
    char buf[0x1000];
    nn::util::SNPrintf(buf, sizeof(buf), "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
    svcOutputDebugString(buf, strlen(buf));
}


void TestAngelScript() {
    char buf[1000];

    nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] TestAngelScript engine startup");
    svcOutputDebugString(buf, strlen(buf));

    asSetGlobalMemoryFunctions(ASAlloc, ASFree);

    // Create the script engine
    asIScriptEngine *engine = asCreateScriptEngine();

    // Set the message callback to receive information on errors in human readable form.
    s32 asErrno = engine->SetMessageCallback(asFUNCTION(ASMessageCallback), 0, asCALL_CDECL); assert( asErrno >= 0 );

    // Register the function that we want the scripts to call
    //asErrno = engine->RegisterGlobalFunction("void TrashPrint(const string &in)", asFUNCTION(ASTrashPrint), asCALL_CDECL); assert( asErrno >= 0 );
    asErrno = engine->RegisterGlobalFunction("void TrashPrintInt(int)", asFUNCTION(ASTrashPrintInt), asCALL_CDECL); assert( asErrno >= 0 );
    const char* scriptText = R"( void main() { int wow = 419; TrashPrintInt(++wow); } )";

    // Create a new script module
    asIScriptModule *mod = engine->GetModule("module", asGM_ALWAYS_CREATE);
    mod->AddScriptSection("script.as", scriptText);

    // Build the module
    asErrno = mod->Build();
    if (asErrno < 0) {
      // The build failed. The message stream will have received
      // compiler errors that shows what needs to be fixed
    }

    // Find the function that is to be called.
    asIScriptFunction *asEntryPoint = mod->GetFunctionByDecl("void main()");
    if (asEntryPoint == nullptr) {
        nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] missing entry point main()");
        svcOutputDebugString(buf, strlen(buf));
        // will crash TODO err handle
    }

    // Create our context, prepare it, and then execute
    asIScriptContext *asCtx = engine->CreateContext();
    asCtx->Prepare(asEntryPoint);
    asErrno = asCtx->Execute();
    if (asErrno != asEXECUTION_FINISHED) {
          // The execution didn't complete as expected. Determine what happened.
        if (asErrno == asEXECUTION_EXCEPTION) {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] uncaught: %s", asCtx->GetExceptionString());
            svcOutputDebugString(buf, strlen(buf));
        }
    }

    // Clean up
    asCtx->Release();
    engine->ShutDownAndRelease();
    nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] engine shutdown");
    svcOutputDebugString(buf, strlen(buf));
}

HOOK_DEFINE_TRAMPOLINE(OnWhistleHook) {
    static const ptrdiff_t s_offset = sym::game::ai::execute::ExecutePlayerWhistle::enterImpl_;

    static void Callback(void* param) {
        TestAngelScript();
        Orig(param);
    }
};

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

        //lotuskit::server::WebSocket::CreateAndWaitForFrontend(); // blocking if enabled
        //WorldManagerModuleBaseProcHook::Install();
        StealHeap::Install();
        OnWhistleHook::Install();


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
