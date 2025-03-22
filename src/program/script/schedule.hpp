#pragma once
#include <string>
#include <angelscript.h>
#include "exlaunch.hpp"
#include "tas/config.hpp"
#include "tas/Playback.hpp"

namespace lotuskit::script::schedule::tas {
    // AS source files are loaded 1:1 into AS modules, allowing scripts to "call into" different files and
    // controlling the flow of execution by suspending and resuming several contexts in a stack-like fashion.
    // We can progressively load+compile+run these on push and free them on pop, allowing for extremely large scripts
    // split+nested across many files within a single logical flow of execution.
    // (This state and control flow do not apply to (tbd) exl hook scripts, only tas scripts.)

    class ModuleStackEntry {
        public:
        AngelScript::asIScriptModule* asModule;
        AngelScript::asIScriptContext* asCtx;
    };
    constexpr size_t MAX_MODULE_STACK_DEPTH = 8;
    inline ModuleStackEntry moduleStack[MAX_MODULE_STACK_DEPTH] = {0};
    inline u8 moduleStackIndex = 0;
    inline ModuleStackEntry* getSP() { return &moduleStack[moduleStackIndex]; }
    inline AngelScript::asIScriptContext* getCtx() { return getSP()->asCtx; }
    void initModuleStack();

    void trySuspendCtx();
    bool hasPlayableCtx();
    bool calcCtx(); // return true if complete/error, false if ok/ongoing

    AngelScript::asIScriptModule* buildOnceOrGetModule(const char* moduleName, const char* sectionName, const char* scriptText);
    void pushExecLocalFileModule(const char* filename);
    void pushExecTextModule(const char* moduleName, const char* sectionName, const char* scriptText, const char* entryPoint);
    void pushExecModuleEntryPoint(AngelScript::asIScriptModule* mod, const char* entryPoint, bool doImmediateExecute = true);

} // ns

