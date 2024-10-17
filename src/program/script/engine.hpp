#pragma once
#include "angelscript.h"
#include "heap/seadHeap.h"

namespace lotuskit::script::engine {
    // AngelScript-wide allocation
    extern sead::Heap* engineHeap;
    inline void* ASAllocImpl(size_t size) { return engineHeap->alloc(size); }
    inline void ASFreeImpl(void* ptr) { engineHeap->free(ptr); }
    void AssignHeap(sead::Heap*);

    //void ASMessageCallback(const AngelScript::asSMessageInfo*, void*);
    void ConfigureEngine(AngelScript::asIScriptEngine*);

    AngelScript::asIScriptModule* TestBuildModule(AngelScript::asIScriptEngine* engine, const char* scriptText);
    void TestExecFuncInNewCtx(AngelScript::asIScriptEngine* engine, AngelScript::asIScriptModule* mod, const char* entryPoint);

} // ns
