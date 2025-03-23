#include "script/engine.hpp"
#include "script/schedule.hpp"
#include "tas/Playback.hpp"
#include "util/fs.hpp"
#include "TextWriter.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

namespace lotuskit::script::schedule::tas {
    void initModuleStack() {
        const auto engine = lotuskit::script::engine::asEngine;
        for (size_t i = 0; i < MAX_MODULE_STACK_DEPTH; i++) {
            moduleStack[i].asCtx = engine->CreateContext();
        }
    }

    void trySuspendCtx() {
        auto* ctx = getCtx();
        if (ctx == nullptr) { return; }
        // assert ctx->GetState() == asEXECUTION_ACTIVE?
        ctx->Suspend();
        // assert ctx->GetState() == asEXECUTION_SUSPENDED
    }

    bool hasPlayableCtx() {
        const auto* ctx = getCtx();
        if (ctx == nullptr) { return false; }
        const auto state = ctx->GetState();
        if (state == AngelScript::asEXECUTION_SUSPENDED) { return true; } // can be resumed
        if (state == AngelScript::asEXECUTION_PREPARED) { return true; } // can be started
        if (state == AngelScript::asEXECUTION_FINISHED && moduleStackIndex > 0) { return true; } // can be popped+resumed
        svcOutputDebugString("badctx", 6); // XXX idk if this happens
        return false;
    }

    bool calcCtx() {
        ModuleStackEntry* sp = nullptr;
        AngelScript::asEContextState prevState;
        s32 asErrno = 0;

        CTX_ENTER:
        if (!hasPlayableCtx()) { abortStack(); return true; } // err
        sp = getSP();
        prevState = sp->asCtx->GetState();
        if (prevState != AngelScript::asEXECUTION_SUSPENDED && prevState != AngelScript::asEXECUTION_PREPARED) { abortStack(); return true; } // err: unexpected state

        asErrno = sp->asCtx->Execute(); // run or resume the ctx until it yields/completes/fails

        if (asErrno == AngelScript::asEXECUTION_FINISHED) {
            if (moduleStackIndex == 0) {
                // entire stack complete
                sp->asCtx->Unprepare();
                tryDiscardLastModuleForPop();
                // TODO run gc?
                return true; // ok: entire script stack complete

            } else {
                // nested scripts
                sp->asCtx->Unprepare();
                tryDiscardLastModuleForPop();
                // TODO run gc?

                // pop ctx, go back and keep running caller (the "await" ends here)
                moduleStackIndex--;
                goto CTX_ENTER;
            }

        } else if (asErrno == AngelScript::asEXECUTION_EXCEPTION) {
            // TODO exceptions
            char buf[1000];
            nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] uncaught: %s", sp->asCtx->GetExceptionString());
            Logger::logText(buf, "/script/engine");
            lotuskit::TextWriter::toastf(30*5, "[error] %s \n", sp->asCtx->GetExceptionString());

            abortStack();
            return true; // err

        } else if (asErrno == AngelScript::asEXECUTION_SUSPENDED) {
            // do not release ongoing async ctx
        }

        return false; // ok: ongoing
    }

    void pushExecLocalFileModule(const char* filename) {
        if (!lotuskit::util::fs::fileExists(filename)) {
            lotuskit::TextWriter::toastf(30*5, "[error] script filename unreadable: %s\n", filename);
            abortStack();
            return;
        }
        // FIXME larger scripts / text alloc
        char scriptBuf[0x2000];
        if (lotuskit::util::fs::readTextFile(scriptBuf, sizeof(scriptBuf), filename)) {
            svcOutputDebugString(scriptBuf, strlen(scriptBuf)); // err
            lotuskit::TextWriter::toastf(30*5, "[error] %s\n", scriptBuf);
            abortStack();
            return;
        }

        auto* mod = buildOnceOrGetModule(filename, filename, scriptBuf);
        if (mod == nullptr) { abortStack(); return; } // err
        // XXX free scriptText
        constexpr bool doImmediateExecute = true;
        pushExecModuleEntryPoint(mod, "void main()", doImmediateExecute);
    }

    void pushExecTextModule(const char* moduleName, const char* sectionName, const char* scriptText, const char* entryPoint) {
        auto* mod = buildOnceOrGetModule(moduleName, sectionName, scriptText);
        if (mod == nullptr) { abortStack(); return; } // err
        // XXX free scriptText
        constexpr bool doImmediateExecute = true;
        pushExecModuleEntryPoint(mod, entryPoint, doImmediateExecute);
    }

    // TODO add push/pop ctx stack for tas scheduling:
    // - allow scheduling arbitrary run lengths by executing on a stack of 1:1:1 ctx:module:file, building+executing+releasing one leaf file at a time
    // - tas::awaitExecScript("sdcard:/totk_lotuskit/page_69_of_420.as"); // suspend currentCtx, build+push+run new file, later resume currentCtx
    // - tas::awaitEvalScript("tas::input(30);"); // XXX would this ever be useful?
    // - tas::awaitExecNXTas("sdcard:/totk_lotuskit/nx-tas.txt"); // TODO transpile+run nxtas source file in new module
    // - pop requires handoff logic in tas::Playback
    // - hotkeys and frontend buttons can work by pushing their payload instead of clobbering TODO needs JsonDispatch for web.as push op
    // - TODO what should break vs push, abort vs pop, etc semantics look like?

    AngelScript::asIScriptModule* buildOnceOrGetModule(const char* moduleName, const char* sectionName, const char* scriptText) {
        const auto engine = lotuskit::script::engine::asEngine;

        // reuse any existing module by name during a single script execution -- do not rebuild/clobber even if scriptText might be different across calls.
        // TODO detect this sooner for file based cases to avoid read?
        AngelScript::asIScriptModule* mod = engine->GetModule(moduleName, AngelScript::asGM_ONLY_IF_EXISTS);
        if (mod != nullptr) { return mod; }

        // build in new module
        mod = engine->GetModule(moduleName, AngelScript::asGM_CREATE_IF_NOT_EXISTS);
        if (mod == nullptr) { return nullptr; } // XXX impossible err?
        mod->AddScriptSection(sectionName, scriptText);
        s32 asErrno = mod->Build(); // Build the module
        if (asErrno < 0) { return nullptr; } // The build failed. The message stream will have received compiler errors that shows what needs to be fixed
        return mod;
    }

    void pushExecModuleEntryPoint(AngelScript::asIScriptModule* mod, const char* entryPoint, bool doImmediateExecute) {
        const auto prevState = getCtx()->GetState();
        if (prevState == AngelScript::asEXECUTION_ACTIVE) {
            // interrupt running script and jump to next entry
            trySuspendCtx();
            moduleStackIndex++;
        } else if (prevState == AngelScript::asEXECUTION_SUSPENDED) {
            // advance sp past sleeping ctx
            moduleStackIndex++;
        } else if (moduleStackIndex == 0) {
            // initial script entry point
        } else {
            abortStack();
            return; // err: sp context is dead
        }

        ModuleStackEntry* sp = getSP();
        if (sp->asCtx == nullptr) { abortStack(); return; } // err should be alloced
        if (sp->asModule != nullptr) { abortStack(); return; } // err should be empty
        // assert sp ctx state is uninitialized/unprepared
        sp->asModule = mod;

        // Find the function that is to be called (mod+entryPoint -> funcptr)
        char buf[500];
        AngelScript::asIScriptFunction *asEntryPoint = mod->GetFunctionByDecl(entryPoint);
        if (asEntryPoint == nullptr) {
            TextWriter::toastf(30*5, "[error] missing entry point %s \n", entryPoint);
            nn::util::SNPrintf(buf, sizeof(buf), "[error] missing entry point %s", entryPoint);
            Logger::logText(buf, "/script/engine");
            abortStack();
            return;
        }

        sp->asCtx->Prepare(asEntryPoint);

        if (doImmediateExecute) {
            const bool isCompleteOrFail = calcCtx();
            if (isCompleteOrFail) { lotuskit::tas::Playback::isPlaybackActive = false; }
        } else {
            // signal that queued ctx should be executed later
            svcOutputDebugString("cant defer", 10); // FIXME
            //lotuskit::tas::Playback::isPlaybackActive = true; // TODO maybe a new flag to request this -- isPlaybackActive makes a bunch of input injection stuff go
        }
    }

    void tryDiscardLastModuleForPop() {
        // wipe last module from sp
        auto* sp = getSP();
        auto* lastModule = sp->asModule;
        if (lastModule == nullptr) { return; }
        sp->asModule = nullptr;

        // only proceed to discard unused modules
        for (int i = 0; i < moduleStackIndex-1; i++) {
            if (moduleStack[i].asModule == lastModule) {
                return; // module is still in use
            }
        }

        lastModule->Discard(); // free
    }

    void abortStack() {
        svcOutputDebugString("as rip", 6); // XXX
        for (size_t i = MAX_MODULE_STACK_DEPTH-1; i > 0; i--) {
            moduleStackIndex = i;
            moduleStack[i].asCtx->Unprepare(); // clear all contexts
            tryDiscardLastModuleForPop(); // free all modules
        }
        moduleStackIndex = 0;
        lotuskit::tas::Playback::isPlaybackActive = false;
    }

} // ns

