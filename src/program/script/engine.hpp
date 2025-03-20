#pragma once
#include "angelscript.h"
#include <heap/seadHeap.h>

namespace lotuskit::script::engine {
    // AngelScript-wide allocation
    extern sead::Heap* engineHeap;
    extern AngelScript::asIScriptEngine* asEngine;
    inline void* asAllocImpl(size_t size) { return engineHeap->alloc(size); }
    inline void asFreeImpl(void* ptr) { engineHeap->free(ptr); }
    void assignHeap(sead::Heap*);

    //void asMessageCallback(const AngelScript::asSMessageInfo*, void*);
    void configureEngine(AngelScript::asIScriptEngine*);
    void createAndConfigureEngine();
    void doAutorun(); // triggered at bootup

    void execLocalFileInScratchModule(const char* filename);
    void execTextInNewModule(const char* moduleName, const char* sectionName, const char* scriptText, const char* entryPoint);
    AngelScript::asIScriptModule* compileToNewModule(AngelScript::asIScriptEngine* engine, const char* moduleName, const char* sectionName, const char* scriptText);
    void execFuncInNewCtx(AngelScript::asIScriptEngine* engine, AngelScript::asIScriptModule* mod, const char* entryPoint);

} // ns

namespace lotuskit::script::engine::ModuleStack {
    // The module stack controls the script's flow of execution by suspending and resuming AS contexts in a stack-like fashion.
    // Each of these contexts might be backed by a separate source AS file, and modules can be freed when popped,
    // making this a (pretty clumsy+inefficient tbh) way to progressively load+compile+run extremely large scripts
    // split+nested across many files within a single flow of execution.
    // (This state and control flow do not apply to (tbd) exl hook scripts, only tas scripts.)

    class ModuleStackEntry {
        public:
        AngelScript::asIScriptModule* asModule;
        AngelScript::asIScriptContext* asCtx;
        // TODO need any sort of status/duration/args/???
        // TODO destruct/release opts?
    };
    constexpr size_t MAX_MODULE_STACK_DEPTH = 8;
    inline ModuleStackEntry stack[MAX_MODULE_STACK_DEPTH] = {0};
    inline u8 moduleStackIndex = 0;
    inline ModuleStackEntry* getCurrent() { return &stack[moduleStackIndex]; }
    inline AngelScript::asIScriptContext* getCurrentCtx() { return getCurrent()->asCtx; }
    inline void pop() {
        auto sp = getCurrent();
        if (moduleStackIndex) { moduleStackIndex--; }
        sp->asCtx = nullptr; // TODO first stop/release ctx if needed?
        sp->asModule = nullptr; // TODO then destruct/release module if needed?
    }
    inline void push(AngelScript::asIScriptModule* asModule, AngelScript::asIScriptContext* asCtx) {
        // not thread safe: don't multi push concurrently, don't push+execute concurrently
        moduleStackIndex++; // XXX assert max depth
        auto sp = getCurrent();
        sp->asModule = asModule;
        sp->asCtx = asCtx;
        // TODO should caller be expected to begin execution? or should tas::Playback::calc automatically begin upon seeing pending asCtx?
    }

} // ns

