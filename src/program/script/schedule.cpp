#include "script/engine.hpp"
#include "script/schedule.hpp"
#include "tas/Playback.hpp"
#include "util/fs.hpp"
#include "util/hash.hpp"
#include "util/pause.hpp"
#include "TextWriter.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;
constexpr auto murmur32 = lotuskit::util::hash::murmur32;


namespace lotuskit::script::schedule::tas {

    bool ModuleStackFrame::isValidForExecution() {
        if (isAbortReq) { return false; }
        if (this->asCtx == nullptr) { return false; }
        if (this->asModule == nullptr) { return false; }

        const auto state = this->asCtx->GetState();
        if (state != AngelScript::asEXECUTION_SUSPENDED && state != AngelScript::asEXECUTION_PREPARED) {
            return false;
        }

        return true;
    }

    bool ModuleStackFrame::calcScheduleIsAwaitPauseRequest() {
        if (this->state.isAwaitPauseRequestHash == 0) { return false; }
        const auto pauseMgr = lotuskit::util::pause::pauseMgr;
        const auto name = pauseMgr->getPauseRequestKey(this->state.isAwaitPauseRequestHash);
        const bool isPause = pauseMgr->getPauseRequestCount(this->state.isAwaitPauseRequestHash) > 0;
        const bool ret = this->state.awaitPauseVal != isPause;
        if (ret) {
            // TODO display pause name
            lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
                textPos->x = 1280.0 - 350.0;
            });
            if (this->state.awaitPauseVal) {
                lotuskit::TextWriter::printf(1, "awaitPauseRequest(%s:%3u)\n               fr:%6d\n", name, lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60-this->state.awaitBegin60), lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60));
            } else {
                lotuskit::TextWriter::printf(1, "awaitUnpauseRequest(%s:%3u)\n               fr:%6d\n", name, lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60-this->state.awaitBegin60), lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60));
            }
        } else {
            this->state.isAwaitPauseRequestHash = 0; // end await
        }
        return ret;
    }

    bool ModuleStackFrame::calcScheduleIsAwaitPauseTarget() {
        if (this->state.isAwaitPauseTargetHash == 0) { return false; }
        const auto pauseMgr = lotuskit::util::pause::pauseMgr;
        const auto name = pauseMgr->getPauseTargetKey(this->state.isAwaitPauseTargetHash);
        const bool isPause = pauseMgr->isTargetPaused(this->state.isAwaitPauseTargetHash);
        const bool ret = this->state.awaitPauseVal != isPause;
        if (ret) {
            lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
                textPos->x = 1280.0 - 350.0;
            });
            if (this->state.awaitPauseVal) {
                lotuskit::TextWriter::printf(1, "tas::awaitPauseTarget(%s:%3u)\n               fr:%6d\n", name, lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60-this->state.awaitBegin60), lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60));
            } else {
                lotuskit::TextWriter::printf(1, "tas::awaitUnpauseTarget(%s:%3u)\n               fr:%6d\n", name, lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60-this->state.awaitBegin60), lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60));
            }
        } else {
            this->state.isAwaitPauseTargetHash = 0; // end await
        }
        return ret;
    }

    bool ModuleStackFrame::isBlockedFromScheduling() {
        // XXX opt to suppress textwriter?
        // XXX elapsedPlayback60 is hard to reason about -- it doesnt include any "loading" or pause await time here, but *some* awaits will likely be desirable to include.
        //     It'll be hard to generalize this to timers/etc if there's not a clear way to describe these different waits, their timekeeping impls, and when to increment them.

        // pending script can never be blocked: eg there is no way to know whether a hotkey payload wants to bypass LoadingPause before we invoke it,
        //                                      so we must bypass everything for pending modules (unless we inherit certain settings from parent sp or some other side channel)
        if (isPlaybackBeginPending) {
            return false;
        }

        //lotuskit::TextWriter::printf(1, "ass(%d)\n", 420); // XXX

        // pause scheduling
        const bool isDebugPause = lotuskit::util::pause::isPauseRequest(0x2d67e2da);
        if (isDebugPause && this->state.skipDebugPause) {
            if (this->state.isAwaitPauseRequestHash == 0x2d67e2da && this->state.awaitPauseVal == true) {
                // do not resume, but if script is awaiting DebugPause request, clear the blocking condition
                this->state.isAwaitPauseRequestHash = 0;
            }
            if (isFrameAdvance && bypassDebugPause60 > 0) {
                //lotuskit::TextWriter::printf(1, "isFrameAdvance(%d) bypassDebugPause60(%d)\n", isFrameAdvance, bypassDebugPause60); // XXX
                // override pause to potentially unblock script
                // return false; // fallthrough
            } else {
                lotuskit::TextWriter::printf(1, "tas::nop(DebugPause)\n               fr:%6d\n", lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60));
                return true;
            }
        }

        const bool isLoadingPause = lotuskit::util::pause::isPauseRequest(0x0eafe200);
        if (isLoadingPause && this->state.skipLoadingPause) {
            if (this->state.isAwaitPauseRequestHash == 0x0eafe200 && this->state.awaitPauseVal == true) {
                // do not resume, but if script is awaiting LoadingPause request, clear the blocking condition
                this->state.isAwaitPauseRequestHash = 0;
            }
            lotuskit::TextWriter::printf(1, "tas::nop(LoadingPause)\n               fr:%6d\n", lotuskit::tas::Playback::duration60ToUIFrames(elapsedPlayback60));
            return true;
        }

        if (this->calcScheduleIsAwaitPauseRequest()) { return true; } // modeline drawn on wait
        if (this->calcScheduleIsAwaitPauseTarget())  { return true; } // modeline drawn on wait

        return false;
    }

    void ModuleStackFrame::resetState() {
        //lotuskit::tas::Playback::PlaybackInput input;
        this->state.inputTTL60 = 0;
        this->state.awaitBegin60 = 0;
        this->state.isAwaitPauseRequestHash = 0;
        this->state.isAwaitPauseTargetHash = 0;
        this->state.skipDebugPause = true;
        this->state.skipLoadingPause = true;
        this->state.awaitPauseVal = false;
        // this->state.playbackInputPassthroughMode = PLAYBACK_TAS_ONLY
    }

    void initModuleStack() {
        const auto engine = lotuskit::script::engine::asEngine;
        for (size_t i = 0; i < MAX_MODULE_STACK_DEPTH; i++) {
            moduleStack[i].asCtx = engine->CreateContext();
        }
    }

    void trySuspendCtx() {
        auto* ctx = getSP()->asCtx;
        if (ctx == nullptr) { return; }
        // assert ctx->GetState() == asEXECUTION_ACTIVE?
        ctx->Suspend();
        // assert ctx->GetState() == asEXECUTION_SUSPENDED
    }

    bool calcConsumeFrametime_queryIsInputExhausted(u32 deltaFrame60) {
        auto* sp = getSP();
        if (isPlaybackBeginPending) {
            // tas playback has not begun, input is not queued yet, but we need to run AS (where it will likely get an input scheduled).
            if (moduleStackIndex > 0) {
                auto* prevsp = &moduleStack[moduleStackIndex-1];
                if (prevsp->state.inputTTL60 > 0) {
                    // For the following frametime calc, deduct from the previous sp's input which was (presumably) just interrupted to schedule the pending sp.
                    // The time which has just passed belonged to it.
                    sp = prevsp;
                }
            }
        }

        // deduct frametime from scheduled input
        auto& state = sp->state;
        if (state.inputTTL60 >= deltaFrame60) {
            state.inputTTL60 -= deltaFrame60;
            elapsedPlayback60 += deltaFrame60;
        } else {
            // only 0/1/2 frames60 left on schedule and deltaFrame60 is larger
            // XXX is erring on one side or the other better, until i properly figure this out?
            //     ie playing out these closing frames and deferring or eating into subsequent inputs? sounds complicated in the dumb way
            state.inputTTL60 = 0; // abandon/truncate the remainder of this scheduled input and resume script
            elapsedPlayback60 += deltaFrame60; // desync observable by user when elapsed time exceeds their expected schedule
        }

        // FIXME we're somehow dropping remainder of input after interrupt? eg try stacking sleeps or stepFrameAdvance interrupt, it skips to next input line...
        // FIXME many menus (eg plus, d-up) dont seem to open when stepping through their button, they seem sensitive to actual press time?

        if (isFrameAdvance) {
            const bool isPause = lotuskit::util::pause::isPauseRequestStr("DebugPause");
            if (!isPause && bypassDebugPause60 == 0) {
                // enforce pause
                lotuskit::util::pause::requestPauseStr("DebugPause");

            } else if (isPause && bypassDebugPause60 == 0) {
                // its paused already yay

            } else if (!state.skipDebugPause) {
                // execution continues without regard to DebugPause -- do not count down bypassDebugPause60, just run

            } else if (isPause && bypassDebugPause60 > 0) {
                // release pause for bypassDebugPause60 duration -- do not count down this frame as we have not run unpaused yet
                lotuskit::util::pause::releasePauseStr("DebugPause");

            } else if (!isPause && bypassDebugPause60 > 0) {
                // countdown time that just elapsed
                bypassDebugPause60 -= std::min(bypassDebugPause60, deltaFrame60);
                if (bypassDebugPause60 == 0) {
                    // enforce pause
                    lotuskit::util::pause::requestPauseStr("DebugPause");
                }
            }
        }

        return state.inputTTL60 == 0; // is scheduled input exhausted?
    }

    bool calcCtx(u32 deltaFrame60) {
        ModuleStackFrame* sp = nullptr;
        s32 asErrno = 0;

        CTX_ENTER:
        sp = getSP();
        if (sp == nullptr || !sp->isValidForExecution()) {
            abortStackReq("[tas::schedule::calcCtx] no playable ctx");
            return true; // err
        }

        // TODO early return for frame advance etc (can we use debugpause?), deduct only frame-advanced time in frametime scheduling.
        //      Exclude isPlaybackBeginPending from early return so it can always execute for eg ws/hotkey during frame advance.

        if (sp->isBlockedFromScheduling()) {
            return false; // ok: currently in an await/etc
        }

        if (deltaFrame60 > 0) {
            if (!calcConsumeFrametime_queryIsInputExhausted(deltaFrame60) && !isPlaybackBeginPending) {
                return false; // ok: keep using scheduled input, do not resume script (unless requested)
            }

            deltaFrame60 = 0; // only deduct frametime before explicit first run
                              // (always 0 on stackframe pop+resume, so we don't deduct duplicate time when we goto CTX_ENTER)
        }

        isPlaybackBeginPending = false;
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

                // pop stackframe, go back and decide whether to resume
                moduleStackIndex--;
                goto CTX_ENTER;
            }

        } else if (asErrno == AngelScript::asEXECUTION_EXCEPTION) {
            // TODO exceptions
            char buf[1000];
            nn::util::SNPrintf(buf, sizeof(buf), "[angelscript] uncaught: %s", sp->asCtx->GetExceptionString());
            Logger::logText(buf, "/script/engine");
            lotuskit::TextWriter::toastf(30*5, "[error] %s \n", sp->asCtx->GetExceptionString());
            abortStackReq("[tas::schedule::calcCtx] exception");
            return true; // err

        } else if (asErrno == AngelScript::asEXECUTION_SUSPENDED) {
            // do not release ongoing async ctx
            return false; // ok: ongoing script yielded (maybe for an await/etc)
        } else if (asErrno == AngelScript::asEXECUTION_ABORTED) {
            return true; // ok: user abort
        }

        svcOutputDebugString("unreachable", 11);
        return true; // err
    }

    void pushExecLocalFileModule(const std::string& filename, const std::string& entryPoint, bool doImmediateExecute, bool skipDebugPause) {
        // reuse any existing module by name during a single script execution -- do not check/rebuild file contents for update
        const auto engine = lotuskit::script::engine::asEngine;
        AngelScript::asIScriptModule* mod = engine->GetModule(filename.c_str(), AngelScript::asGM_ONLY_IF_EXISTS);

        if (mod == nullptr) {
            // read+build new module
            if (!lotuskit::util::fs::fileExists(filename.c_str())) {
                lotuskit::TextWriter::toastf(30*5, "[error] script filename unreadable: %s\n", filename.c_str());
                abortStackReq("[tas::schedule::pushExecLocalFileModule] unreadable");
                return;
            }

            // FIXME larger scripts / text alloc
            char scriptBuf[0x2000];
            if (lotuskit::util::fs::readTextFile(scriptBuf, sizeof(scriptBuf), filename.c_str())) {
                svcOutputDebugString(scriptBuf, strlen(scriptBuf));
                abortStackReq("[tas::schedule::pushExecLocalFileModule] unreadable");
                return;
            }
            mod = buildOnceOrGetModule(filename, filename, scriptBuf);
            // XXX free scriptBuf
        }

        if (mod == nullptr) {
            abortStackReq("[tas::schedule::pushExecLocalFileModule] module failed to build");
            return; // err
        }
        pushExecModuleEntryPoint(mod, entryPoint, doImmediateExecute, skipDebugPause);
    }

    void pushExecLocalFileModuleNXTas(const std::string& filename, bool doImmediateExecute, bool skipDebugPause) {
        // reuse any existing module by name during a single script execution -- do not check/rebuild file contents for update
        const auto engine = lotuskit::script::engine::asEngine;
        AngelScript::asIScriptModule* mod = engine->GetModule(filename.c_str(), AngelScript::asGM_ONLY_IF_EXISTS);

        if (mod == nullptr) {
            // read+build new module
            if (!lotuskit::util::fs::fileExists(filename.c_str())) {
                lotuskit::TextWriter::toastf(30*5, "[error] script filename unreadable: %s\n", filename.c_str());
                abortStackReq("[tas::schedule::pushExecLocalFileModuleNXTas] unreadable");
                return;
            }

            // FIXME larger scripts / text alloc
            char nxtasBuf[0x2000]; nxtasBuf[0] = '\0'; // ro source nxtas
            char scriptBuf[0x2000]; scriptBuf[0] = '\0'; // wx output AS
            if (lotuskit::util::fs::readTextFile(nxtasBuf, sizeof(nxtasBuf), filename.c_str())) {
                svcOutputDebugString(nxtasBuf, strlen(nxtasBuf));
                abortStackReq("[tas::schedule::pushExecLocalFileModuleNXTas] unreadable");
                return;
            }
            transpileImpl_nxtas_to_as(nxtasBuf, scriptBuf, sizeof(scriptBuf));
            // XXX free nxtasBuf
            mod = buildOnceOrGetModule(filename, filename, scriptBuf);
            // XXX free scriptBuf
        }

        if (mod == nullptr) {
            abortStackReq("[tas::schedule::pushExecLocalFileModuleNXTas] module failed to build");
            return; // err
        }
        pushExecModuleEntryPoint(mod, "void main()", doImmediateExecute, skipDebugPause);
    }

    void pushExecTextModule(const std::string& moduleName, const std::string& sectionName, const std::string& scriptText, const std::string& entryPoint, bool doImmediateExecute, bool skipDebugPause) {
        auto* mod = buildOnceOrGetModule(moduleName, sectionName, scriptText);
        if (mod == nullptr) { abortStackReq("[tas::schedule::pushExecTextModule] module failed to build"); return; } // err
        // XXX free scriptText
        pushExecModuleEntryPoint(mod, entryPoint, doImmediateExecute, skipDebugPause);
    }

    void pushExecEval(const std::string& scriptText, const std::string& entryPoint, bool doImmediateExecute, bool skipDebugPause) {
        char moduleName[32];
        nn::util::SNPrintf(moduleName, sizeof(moduleName), "eval_%08x.as", murmur32(scriptText));
        pushExecTextModule(moduleName, moduleName, scriptText, entryPoint, doImmediateExecute, skipDebugPause);
    }

    void pushExecModuleEntryPoint(AngelScript::asIScriptModule* mod, const std::string& entryPoint, bool doImmediateExecute, bool skipDebugPause) {
        const auto prevState = getSP()->asCtx->GetState();
        const auto* prevMod = getSP()->asModule;

        if (prevMod != nullptr) {
            // sp slot occupied, advance past it
            if (prevState == AngelScript::asEXECUTION_ACTIVE) {
                trySuspendCtx();
            }
            moduleStackIndex++;

        } else {
            // sp slot is empty, just reuse it
            if (prevState == AngelScript::asEXECUTION_ACTIVE) {
                trySuspendCtx(); // XXX idk if necessary
            }
        }

        ModuleStackFrame* sp = getSP();
        if (sp->asCtx == nullptr) { abortStackReq("[tas::schedule::pushExecModuleEntryPoint] sp ctx null"); return; } // err should be alloced
        if (sp->asModule != nullptr) { abortStackReq("[tas::schedule::pushExecModuleEntryPoint] sp module not null"); return; } // err should be empty
        // XXX assert asCtx state is uninitialized/unprepared
        sp->asModule = mod;
        sp->resetState(); // wipe timing/async/etc data
        sp->state.skipDebugPause = skipDebugPause;

        // Find the function that is to be called (mod+entryPoint -> funcptr)
        char buf[500];
        AngelScript::asIScriptFunction *asEntryPoint = mod->GetFunctionByDecl(entryPoint.c_str());
        if (asEntryPoint == nullptr) {
            TextWriter::toastf(30*5, "[error] missing entry point %s \n", entryPoint.c_str());
            nn::util::SNPrintf(buf, sizeof(buf), "[error] missing entry point %s", entryPoint.c_str());
            Logger::logText(buf, "/script/engine");
            abortStackReq("[tas::schedule::pushExecModuleEntryPoint] cant resolve entrypoint");
            return;
        }

        sp->asCtx->Prepare(asEntryPoint);

        if (doImmediateExecute) {
            const bool isCompleteOrFail = calcCtx();
            if (isCompleteOrFail) { isPlaybackActive = false; }
        } else {
            // signal that queued ctx should be executed in main loop tas calc
            isPlaybackBeginPending = true;
        }
    }

    AngelScript::asIScriptModule* buildOnceOrGetModule(const std::string& moduleName, const std::string& sectionName, const std::string& scriptText) {
        const auto engine = lotuskit::script::engine::asEngine;

        // reuse any existing module by name during a single script execution -- do not rebuild/clobber even if scriptText might be different across calls.
        AngelScript::asIScriptModule* mod = engine->GetModule(moduleName.c_str(), AngelScript::asGM_ONLY_IF_EXISTS);
        if (mod != nullptr) { return mod; }

        // build in new module
        mod = engine->GetModule(moduleName.c_str(), AngelScript::asGM_CREATE_IF_NOT_EXISTS);
        if (mod == nullptr) { return nullptr; } // XXX impossible err?
        mod->AddScriptSection(sectionName.c_str(), scriptText.c_str());
        s32 asErrno = mod->Build(); // Build the module
        if (asErrno < 0) { return nullptr; } // The build failed. The message stream will have received compiler errors that shows what needs to be fixed
        return mod;
    }

    void tryDiscardLastModuleForPop() {
        // wipe last module from sp
        auto* sp = getSP();
        auto* lastModule = sp->asModule;
        if (lastModule == nullptr) { return; }
        sp->asModule = nullptr;

        // only proceed to discard unused modules
        s32 n = moduleStackIndex - 1;
        for (s32 i = 0; i < n; i++) {
            // assert(moduleStackIndex != 0, "unreachable: comparing last module to only module"); // lone module 0 can never early return, it is always freed
            if (moduleStack[i].asModule == lastModule) {
                return; // module is still in use
            }
        }

        lastModule->Discard(); // free
    }

    void abortStackReq(const std::string& reason) {
        isAbortReq = true;
        abortReqReason = reason;
        for (size_t i = 0; i < MAX_MODULE_STACK_DEPTH; i++) {
            const auto* sp = &moduleStack[i];
            if (sp->asCtx) { sp->asCtx->Abort(); } // abort all contexts
        }
    }

    void abortStackImpl() {
        if (!isAbortReq) { return; } // nop
        isAbortReq = false;
        // assert not within AS execution

        // scream
        char buf[420];
        nn::util::SNPrintf(buf, sizeof(buf), "[script] abort: %s", abortReqReason.c_str());
        svcOutputDebugString(buf, strlen(buf));
        TextWriter::toastf(30*5, "%s\n", buf);
        dumpStack();

        // throw it all away
        for (size_t i = 0; i < MAX_MODULE_STACK_DEPTH; i++) {
            auto* sp = &moduleStack[i];
            if (sp->asCtx) { sp->asCtx->Abort(); sp->asCtx->Unprepare(); } // clear all contexts
            if (sp->asModule) { sp->asModule->Discard(); sp->asModule = nullptr; } // free all modules
        }
        moduleStackIndex = 0;
        isPlaybackActive = false;
        abortReqReason = "";
        // TODO run gc?
    }

    void dumpStack() {
        char buf[420];
        for (size_t i = 0; i < MAX_MODULE_STACK_DEPTH; i++) {
            const auto* sp = &moduleStack[i];
            std::string cs = "?";
            if (sp->asCtx) {
                // https://www.angelcode.com/angelscript/sdk/docs/manual/angelscript_8h.html#a867f14b4137dd4602fda1e616b217a69
                const auto state = sp->asCtx->GetState();
                if (state == AngelScript::asEXECUTION_FINISHED)  { cs = "finished"; }
                if (state == AngelScript::asEXECUTION_SUSPENDED) { cs = "suspended"; }
                if (state == AngelScript::asEXECUTION_ABORTED)   { cs = "aborted"; }
                if (state == AngelScript::asEXECUTION_EXCEPTION) { cs = "exception"; }
                if (state == AngelScript::asEXECUTION_PREPARED)  { cs = "prepared"; }
                if (state == AngelScript::asEXECUTION_UNINITIALIZED) { cs = "uninitialized"; }
                if (state == AngelScript::asEXECUTION_ACTIVE)    { cs = "active"; }
                if (state == AngelScript::asEXECUTION_ERROR)     { cs = "error"; }
                if (state == AngelScript::asEXECUTION_DESERIALIZATION) { cs = "deserialization"; }
            }
            const auto* mod = sp->asModule;
            const char* modName = mod ? mod->GetName() : "null";
            if (i == moduleStackIndex) {
                nn::util::SNPrintf(buf, sizeof(buf), "tas::stack[sp]: ctx(%s:%p) mod(%s:%p)", cs.c_str(), sp->asCtx, modName, mod);
            } else {
                nn::util::SNPrintf(buf, sizeof(buf), "tas::stack[%02d]: ctx(%s:%p) mod(%s:%p)", i, cs.c_str(), sp->asCtx, modName, mod);
            }
            svcOutputDebugString(buf, strlen(buf));
            TextWriter::toastf(30*5, "%s\n", buf);
        }
    }

    void transpileImpl_nxtas_to_as(const char* src, char* dst, size_t dstMax) {
        // Converts nxtas to AS intermediate (not really meant for human editing, eg raw button flags).
        // Input must be well-formed! Excess whitespace between token groups is generally tolerated, not much else nonsense.

        // assert dstMax isnt silly small
        const char* entrypoint = "void main()";
        const char* entrypoint_open = " {\n";
        const char* entrypoint_close = "}\n";
        // accumulators
        u32 accDuration = 0;
        u64 accButton = 0;
        s32 accStick[4] = {0};
        // file offsets
        size_t dstn = 0;
        // parse state
        u64 lineButton = 0;
        s32 lineStick[4] = {0};
        s32 skipAheadTmpForScanf = 0;
        char dstTmp[200]; dstTmp[0] = '\0'; dstTmp[199] = '\0';

        // open func
        strcpy(dst, entrypoint); dst += strlen(entrypoint); dstn += strlen(entrypoint);
        strcpy(dst, entrypoint_open); dst += strlen(entrypoint_open); dstn += strlen(entrypoint_open);
        const size_t limit = dstMax - strlen(entrypoint_close) - 1; // reserve entrypoint_close+terminate
        do {
            // begin source line: skip frame counter, leading whitespace, empty lines
            while (true) {
                while (*src == ' ' || *src == '\t' || *src == '\r' || *src == '\n') { src++; goto CONTINUE_2; } // consume whitespace
                while (*src >= '0' && *src <= '9') { src++; goto CONTINUE_2; } // consume uint
                break;
                CONTINUE_2:
            }
            if (*src == '\0') { break; } // end src


            lineButton = 0;
            while (true) { // parse each button flag
                // TODO option to preserve enums in output? but i dont wanna convert that, would prob rather just s/;/,/g the source flags... but effort, very different control flow.
                //      (eg just inspecting the string would incorrectly proc accumulation when flag order changes) Shouldnt be worth the effort as long as this remains an intermediate output only?
                if (strncmp(src, "NONE",        4) == 0) { lineButton  =       0; src +=  4; break; } // assert no other button flags with NONE
                if (strncmp(src, "KEY_A",       5) == 0) { lineButton |= 1 <<  0; src +=  5; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_B",       5) == 0) { lineButton |= 1 <<  1; src +=  5; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_X",       5) == 0) { lineButton |= 1 <<  2; src +=  5; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_Y",       5) == 0) { lineButton |= 1 <<  3; src +=  5; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_ZL",      6) == 0) { lineButton |= 1 <<  8; src +=  6; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_ZR",      6) == 0) { lineButton |= 1 <<  9; src +=  6; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_PLUS",    8) == 0) { lineButton |= 1 << 10; src +=  8; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_MINUS",   9) == 0) { lineButton |= 1 << 11; src +=  9; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_DLEFT",   9) == 0) { lineButton |= 1 << 12; src +=  9; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_DUP",     7) == 0) { lineButton |= 1 << 13; src +=  7; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_DRIGHT", 10) == 0) { lineButton |= 1 << 14; src += 10; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_DDOWN",   9) == 0) { lineButton |= 1 << 15; src +=  9; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_LSTICK", 10) == 0) { lineButton |= 1 <<  4; src += 10; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_RSTICK", 10) == 0) { lineButton |= 1 <<  5; src += 10; if (*src == ';') { src++; continue; } }
                if (strncmp(src, "KEY_L",       5) == 0) { lineButton |= 1 <<  6; src +=  5; if (*src == ';') { src++; continue; } } // these must be last so they dont shadow LSTICK+RSTICK
                if (strncmp(src, "KEY_R",       5) == 0) { lineButton |= 1 <<  7; src +=  5; if (*src == ';') { src++; continue; } }
                break;
            }

            // parse sticks
            while (*src == ' ' || *src == '\t') { src++; } // consume whitespace
            sscanf(src, "%d;%d%n", &lineStick[0], &lineStick[1], &skipAheadTmpForScanf); // read LStick
            src += skipAheadTmpForScanf; // consume LStick
            while (*src == ' ' || *src == '\t') { src++; } // consume whitespace
            sscanf(src, "%d;%d%n", &lineStick[2], &lineStick[3], &skipAheadTmpForScanf); // read RStick
            src += skipAheadTmpForScanf; // consume RStick

            // source line complete, do accumulate+write
            if (accDuration++ == 0) {
                // begin accumulating this input (first iteration)
                accButton   = lineButton;
                accStick[0] = lineStick[0];
                accStick[1] = lineStick[1];
                accStick[2] = lineStick[2];
                accStick[3] = lineStick[3];
                continue; // proceed to next source line
            }
            if (accButton   != lineButton   ||
                accStick[0] != lineStick[0] ||
                accStick[1] != lineStick[1] ||
                accStick[2] != lineStick[2] ||
                accStick[3] != lineStick[3] ) {

                // emit accumulated input
                nn::util::SNPrintf(dstTmp, sizeof(dstTmp), "tas::input(%d, %llu, %d,%d, %d,%d);\n", accDuration, accButton, accStick[0], accStick[1], accStick[2], accStick[3]);
                dstn += strlen(dstTmp);
                if (dstn >= limit) { svcOutputDebugString("xpile overflow", 14); break; } // err
                strcpy(dst, dstTmp);
                dst += strlen(dstTmp);

                // begin accumulating new input
                accDuration = 1;
                accButton   = lineButton;
                accStick[0] = lineStick[0];
                accStick[1] = lineStick[1];
                accStick[2] = lineStick[2];
                accStick[3] = lineStick[3];
            }

        } while(dstn < limit);

        // emit final accumulated input
        if (accDuration > 0) {
            nn::util::SNPrintf(dstTmp, sizeof(dstTmp), "tas::input(%d, %llu, %d,%d, %d,%d);\n", accDuration, accButton, accStick[0], accStick[1], accStick[2], accStick[3]);
            dstn += strlen(dstTmp);
            if (dstn >= limit) { svcOutputDebugString("xpile overflow", 14); } // err
            else {
                strcpy(dst, dstTmp);
                dst += strlen(dstTmp);
            }
        }

        // close func
        strcpy(dst, entrypoint_close); dst += strlen(entrypoint);
        *dst = '\0'; // terminate
    }

} // ns

