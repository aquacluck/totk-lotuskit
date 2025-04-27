#pragma once
#include <string>
#include <angelscript.h>
#include "exlaunch.hpp"
#include "tas/Playback.hpp"

namespace lotuskit::script::schedule::tas {
    // AS source files are loaded 1:1 into AS modules, allowing scripts to "call into" different files and
    // controlling the flow of execution by suspending and resuming several contexts in a stack-like fashion.
    // We can progressively load+compile+run these on push and free them on pop, allowing for extremely large scripts
    // split+nested across many files within a single logical flow of execution.
    // (This state and control flow do not apply to (tbd) exl hook scripts, only tas scripts.)

    // global scheduling state
    inline u32 elapsedPlayback60 = 0; // 30fps half-frames elapsed this script execution (based on counting)
    inline bool isPlaybackActive = false; // tas is operational (note this does not imply `ctx->GetState() == asEXECUTION_ACTIVE`, AS does not constantly run)
    inline bool isPlaybackBeginPending = false;
    inline bool isAbortReq = false;
    inline std::string abortReqReason = "";

    class ModuleStackFrameScheduleState {
        public:
        // frametime scheduling
        lotuskit::tas::Playback::PlaybackInput input;
        u32 inputTTL60; // remaining 30fps half-frames to apply scheduled TAS input

        // pause scheduling
        // TODO extract some interface to register awaitable calls?
        u32 awaitBegin60; // common: for time spent waiting, relative to elapsedPlayback60
        u32 isAwaitPauseRequestHash;
        u32 isAwaitPauseTargetHash;
        bool skipLoadingPause; // frames with LoadingPause do not count toward input timing
        bool awaitPauseVal; // common: waiting for pause vs unpause

        // TODO store+revert to correct passthrough mode on pop (eg if a hotkey suspends a tas::input and plays other inputs)
        // lotuskit::tas::config::PlaybackInputPassthroughMode playbackInputPassthroughMode;
    };

    class ModuleStackFrame {
        public:
        AngelScript::asIScriptModule* asModule;
        AngelScript::asIScriptContext* asCtx;
        ModuleStackFrameScheduleState state;

        bool isValidForExecution(); // is allocated/prepared/etc? (unconcerned with scheduling)
        bool isBlockedFromScheduling(); // any logic/await blocking execution? (blocked scripts are still "valid for execution")
        void resetState();

        private:
        bool calcScheduleIsAwaitPauseRequest();
        bool calcScheduleIsAwaitPauseTarget();
    };

    // stack impl
    constexpr size_t MAX_MODULE_STACK_DEPTH = 12;
    inline ModuleStackFrame moduleStack[MAX_MODULE_STACK_DEPTH] = {0}; // XXX defer alloc to initModuleStack?
    inline u8 moduleStackIndex = 0;
    inline ModuleStackFrame* getSP() { return &moduleStack[moduleStackIndex]; }
    void initModuleStack(); // init AS contexts at bootup

    // main control flow
    bool calcCtx(u32 deltaFrame60 = 0); // return true if complete/error, false if ok/ongoing
    bool calcConsumeFrametime_queryIsInputExhausted(u32 deltaFrame60);
    void tryDiscardLastModuleForPop();
    void trySuspendCtx();

    // push/enqueue
    AngelScript::asIScriptModule* buildOnceOrGetModule(const std::string& moduleName, const std::string& sectionName, const std::string& scriptText);
    void pushExecLocalFileModule(const std::string& filename, const std::string& entryPoint, bool doImmediateExecute = true);
    void pushExecLocalFileModuleNXTas(const std::string& filename, bool doImmediateExecute = true); // transpile+run nxtas source file in new module
    void pushExecTextModule(const std::string& moduleName, const std::string& sectionName, const std::string& scriptText, const std::string& entryPoint, bool doImmediateExecute = true);
    void pushExecEval(const std::string& scriptText, const std::string& entryPoint, bool doImmediateExecute = true); // sugar for AS
    void pushExecModuleEntryPoint(AngelScript::asIScriptModule* mod, const std::string& entryPoint, bool doImmediateExecute = true);

    // util
    void abortStackReq(const std::string& reason = "err");
    void abortStackImpl(); // process abort req
    void transpileImpl_nxtas_to_as(const char* src, char* dst, size_t dstMax); // XXX doesnt belong here but it'll be easier to fix up alloc stuff when its not scattered (translating/loading/etc large scripts is very fragile, prob because it uses fakeheap)
    void dumpStack();

} // ns

