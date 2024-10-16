#pragma once
#include "nn/util.h"
#include "angelscript.h"

namespace lotuskit::script::globals {
    //void ASTrashPrint(auto &msg) {
    //    const char* str = msg.c_str();
    //    svcOutputDebugString(str, strlen(str));
    //}
    void ASTrashPrintInt(int num) {
        char buf[1000];
        nn::util::SNPrintf(buf, sizeof(buf), "oink %d", num);
        svcOutputDebugString(buf, strlen(buf));
    }
    void RegisterGlobals(AngelScript::asIScriptEngine* engine) {
        s32 asErrno = engine->RegisterGlobalFunction("void TrashPrintInt(int)", AngelScript::asFUNCTION(ASTrashPrintInt), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );
        //asErrno = engine->RegisterGlobalFunction("void TrashPrint(const string &in)", AngelScript::asFUNCTION(ASTrashPrint), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );
    }
} // ns
