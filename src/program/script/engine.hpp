#pragma once
#include "angelscript.h"
#include "heap/seadHeap.h"

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
    void execInScratchModule(const char* scriptText);
    AngelScript::asIScriptModule* compileToScratchModule(AngelScript::asIScriptEngine* engine, const char* scriptText);
    void execFuncInNewCtx(AngelScript::asIScriptEngine* engine, AngelScript::asIScriptModule* mod, const char* entryPoint);

} // ns
