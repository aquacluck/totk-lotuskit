#include "script/engine.hpp"
#include "script/globals.hpp"
#include "nn/util.h"
#include "lib.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

#include "angelscript.h"
#include "scriptarray.h"
#include "scriptmath.h"
//#include "scriptmathcomplex.h"
#include "scriptstdstring.h"

namespace lotuskit::script::engine {
    // AngelScript-wide allocation
    sead::Heap* engineHeap = nullptr;
    AngelScript::asIScriptEngine* asEngine = nullptr;

    void assignHeap(sead::Heap* heap) {
        engineHeap = heap;
        AngelScript::asSetGlobalMemoryFunctions(asAllocImpl, asFreeImpl);
    }

    void asMessageCallback(const AngelScript::asSMessageInfo* msg, void* param) {
        const char *type = "ERR ";
        if (msg->type == AngelScript::asMSGTYPE_WARNING) {
            type = "WARN ";
        } else if(msg->type == AngelScript::asMSGTYPE_INFORMATION) {
            type = "INFO ";
        }

        char buf[0x1000];
        nn::util::SNPrintf(buf, sizeof(buf), "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
        Logger::logText(buf, "/script/engine");
    }

    void configureEngine(AngelScript::asIScriptEngine* engine) {
        // do more unsafe C stuff
        engine->SetEngineProperty(AngelScript::asEP_ALLOW_MULTILINE_STRINGS, true);
        engine->SetEngineProperty(AngelScript::asEP_ALLOW_UNSAFE_REFERENCES, true);
        engine->SetEngineProperty(AngelScript::asEP_COPY_SCRIPT_SECTIONS, false); // caller owns script text throughout asIScriptModule::Build
        engine->SetEngineProperty(AngelScript::asEP_USE_CHARACTER_LITERALS, true);
        // i'm too dumb to deal with magic
        engine->SetEngineProperty(AngelScript::asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT, 2); // never
        engine->SetEngineProperty(AngelScript::asEP_ALWAYS_IMPL_DEFAULT_COPY_CONSTRUCT, 2); // never
        engine->SetEngineProperty(AngelScript::asEP_ALWAYS_IMPL_DEFAULT_COPY, 2); // never
        engine->SetEngineProperty(AngelScript::asEP_ALTER_SYNTAX_NAMED_ARGS, 2); // do not warn = vs : usage for kwarg syntax
        engine->SetEngineProperty(AngelScript::asEP_GENERIC_CALL_MODE, 1); // XXX handle ref counting: "treat them the same way as native calling convention", sounds like what i would want?
                                                                           // Really i just want raw pointers + no implicit construction/alloc/etc, but can't have it
        // non-preemptive wrt script, scripts should yield to return control to the game:
        // (script timeouts are also possible, and the underlying guest thread could still be interrupted, but generally continuously runs+blocks the thread until script yields.)
        // XXX ensure we can run other parts of script module with an existing context suspended -- tas+ws runner(s?) should be independent of eg hooking AS callbacks.
        // - tas+ws scripts are intended to run asap/realtime and so they're executed in WorldManagerModule::baseProcExe. Offloading this to another thread isn't really desirable.
        //     - exception: http://www.angelcode.com/angelscript/sdk/docs/manual/doc_adv_dynamic_build.html#doc_adv_dynamic_build_incr
        //       We'll want to page in/out long tas scripts somehow, doing this in another thread is good for both the disk io and AS compile work.
        // - hook callback scripts need to run wherever they're invoked from. These may need to acquire a single asIScriptContext, until we can glue the TLS stuff needed for proper threading.
        engine->SetEngineProperty(AngelScript::asEP_AUTO_GARBAGE_COLLECT, false); // TODO get a proper child sead::Heap, get some benchmarks, try default/full/onestep/etc
        engine->SetEngineProperty(AngelScript::asEP_DISABLE_SCRIPT_CLASS_GC, true); // surely i wouldnt make circular references :D
        engine->SetEngineProperty(AngelScript::asEP_BUILD_WITHOUT_LINE_CUES, true);

        // Set the message callback to receive information on errors in human readable form.
        s32 asErrno = engine->SetMessageCallback(AngelScript::asFUNCTION(asMessageCallback), 0, AngelScript::asCALL_CDECL); assert( asErrno >= 0 );

        // addons
        AngelScript::RegisterScriptArray(engine, true);
        AngelScript::RegisterScriptMath(engine);
        //AngelScript::RegisterScriptMathComplex(engine);
        AngelScript::RegisterStdString(engine);
        AngelScript::RegisterStdStringUtils(engine);
    }

    void createAndConfigureEngine() {
        asEngine = AngelScript::asCreateScriptEngine();
        configureEngine(asEngine);
        lotuskit::script::globals::registerGlobals(asEngine);
        // asEngine->ShutDownAndRelease();
    }

    void execInScratchModule(const char* scriptText) {
        auto* mod = lotuskit::script::engine::compileToScratchModule(asEngine, scriptText);
        const char* entryPoint = "void main()";
        lotuskit::script::engine::execFuncInNewCtx(asEngine, mod, entryPoint);
    }

    AngelScript::asIScriptModule* compileToScratchModule(AngelScript::asIScriptEngine* engine, const char* scriptText) {
        // Create a new script module
        AngelScript::asIScriptModule* mod = engine->GetModule("scratch", AngelScript::asGM_ALWAYS_CREATE); // clobber existing
        mod->AddScriptSection("script.as", scriptText);
        s32 asErrno = mod->Build(); // Build the module
        if (asErrno < 0) {
            // The build failed. The message stream will have received
            // compiler errors that shows what needs to be fixed
        }
        return mod;
    }

    void execFuncInNewCtx(AngelScript::asIScriptEngine* engine, AngelScript::asIScriptModule* mod, const char* entryPoint) {
        char buf[500];

        // Find the function that is to be called (mod+entryPoint -> funcptr)
        AngelScript::asIScriptFunction *asEntryPoint = mod->GetFunctionByDecl(entryPoint);
        if (asEntryPoint == nullptr) {
            nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] missing entry point %s", entryPoint);
            Logger::logText(buf, "/script/engine");
            return;
        }

        // Create our context, prepare it, and then execute (engine -> ctx, + funcptr -> exec)
        AngelScript::asIScriptContext *asCtx = engine->CreateContext();
        asCtx->Prepare(asEntryPoint);
        s32 asErrno = asCtx->Execute();

        if (asErrno == AngelScript::asEXECUTION_FINISHED) {
            asCtx->Release();

        } else if (asErrno == AngelScript::asEXECUTION_EXCEPTION) {
            // TODO exceptions
            nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] uncaught: %s", asCtx->GetExceptionString());
            Logger::logText(buf, "/script/engine");
            asCtx->Release();

        } else if (asErrno == AngelScript::asEXECUTION_SUSPENDED) {
            // do not release ongoing async ctx
        }
    }
} // ns
