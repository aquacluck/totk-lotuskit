#pragma once
#include <angelscript.h>
#include "exlaunch.hpp"
#include "tas/Playback.hpp"

namespace lotuskit::script::schedule::tas {
    // AS source files are loaded 1:1 into AS modules, allowing scripts to "call into" different files and
    // controlling the flow of execution by suspending and resuming several contexts in a stack-like fashion.
    // We can progressively load+compile+run these on push and free them on pop, allowing for extremely large scripts
    // split+nested across many files within a single logical flow of execution.
    // (This state and control flow do not apply to (tbd) exl hook scripts, only tas scripts.)

    enum class InputDurationScalingStrategy {
        // how inputs should be dumped, or how playback commands should be interpreted
        FPS60_1X = 1, // awkward but 1:1: count in raw 30fps half-frames
        FPS30_2X = 2, // default: 1 frame = 30fps intended
        FPS20_3X = 3, // awkward and useless? assume we're pegged at 20fps and this is the new normal
    };

    enum class PlaybackInputPassthroughMode {
        NULL_VANILLA = 0,      // full passthrough ("sleep" or human only)
        PLAYBACK_TAS_ONLY = 1, // input supplied verbatim by tas
        PASSTHROUGH_OR = 2,    // human inputs added to tas inputs: human may add buttons or alter axes
        PASSTHROUGH_XOR = 3,   // human inputs invert tas inputs: human may toggle buttons or cancel out axes
    };

    // global scheduling state
    inline u32 elapsedPlayback60 = 0; // 30fps half-frames elapsed this script execution (based on counting)
    inline u32 bypassDebugPause60 = 0; // frames to step ahead in tas playback, overriding normal pause scheduling
    inline bool isPlaybackActive = false; // tas is operational (note this does not imply `ctx->GetState() == asEXECUTION_ACTIVE`, AS does not constantly run)
    inline bool isPlaybackBeginPending = false;
    inline bool isAbortReq = false;
    inline bool isFrameAdvance = false; // hold DebugPause by default, momentarily unpause with bypassDebugPause60
    inline String abortReqReason = "";

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
        bool blockOnDebugPause; // frames with DebugPause do not count toward input timing
        bool blockOnLoadingPause; // frames with LoadingPause do not count toward input timing
        bool awaitPauseVal; // common: waiting for pause vs unpause

        InputDurationScalingStrategy inputFPSMode;
        PlaybackInputPassthroughMode playbackInputPassthroughMode;
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
    inline ModuleStackFrame* moduleStack = nullptr;
    inline u8 moduleStackIndex = 0;
    inline ModuleStackFrame* getSP() { return &moduleStack[moduleStackIndex]; }
    void initModuleStack(); // init AS contexts at bootup

    // main control flow
    bool calcCtx(u32 deltaFrame60 = 0); // return true if complete/error, false if ok/ongoing
    bool calcConsumeFrametime_queryIsInputExhausted(u32 deltaFrame60);
    void tryDiscardLastModuleForPop();
    void trySuspendCtx();

    // push/enqueue
    AngelScript::asIScriptModule* buildOnceOrGetModule(const String& moduleName, const String& sectionName, const String& scriptText);
    void pushExecLocalFileModule(const String& filename, const String& entryPoint, bool doImmediateExecute = true, bool blockOnDebugPause = true);
    void pushExecLocalFileModuleNXTas(const String& filename, bool doImmediateExecute = true, bool blockOnDebugPause = true); // transpile+run nxtas source file in new module
    void pushExecTextModule(const String& moduleName, const String& sectionName, const String& scriptText, const String& entryPoint, bool doImmediateExecute = true, bool blockOnDebugPause = true);
    void pushExecEval(const String& scriptText, const String& entryPoint, bool doImmediateExecute = true, bool blockOnDebugPause = true); // sugar for AS
    void pushExecModuleEntryPoint(AngelScript::asIScriptModule* mod, const String& entryPoint, bool doImmediateExecute = true, bool blockOnDebugPause = true);

    // util
    void abortStackReq(const String& reason = "err");
    void abortStackImpl(); // process abort req
    void transpileImpl_nxtas_to_as(const char* src, char* dst, size_t dstMax); // XXX doesnt belong here but it'll be easier to fix up alloc stuff when its not scattered (translating/loading/etc large scripts is very fragile, prob because it uses fakeheap)
    void dumpStack();
    u32 duration60ToUIFrames(u32 duration60, InputDurationScalingStrategy inputFPSMode);

} // ns

