#pragma once
#include "nn/util.h"
#include "angelscript.h"
#include "lib/json.hpp"
#include "Logger.hpp"

using json = nlohmann::json;
using Logger = lotuskit::Logger;

namespace lotuskit::script::globals {
    //void ASTrashPrint(auto &msg) {
    //    const char* str = msg.c_str();
    //    svcOutputDebugString(str, strlen(str));
    //}
    void ASTrashPrintInt(int num) {
        Logger::logJson(json::object({ {"oink", num} }), "/script/TrashPrintInt");
    }
    void RegisterGlobals(AngelScript::asIScriptEngine* engine) {
        //engine->SetDefaultNamespace("Logger");
        //engine->RegisterGlobalFunction("void logText(string, string)", AngelScript::asFUNCTION(Logger::logText), AngelScript::asCALL_CDECL);
        s32 asErrno = engine->RegisterGlobalFunction("void TrashPrintInt(int)", AngelScript::asFUNCTION(ASTrashPrintInt), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );
        //asErrno = engine->RegisterGlobalFunction("void TrashPrint(const string &in)", AngelScript::asFUNCTION(ASTrashPrint), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );
    }
} // ns
