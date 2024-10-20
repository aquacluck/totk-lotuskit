#pragma once
#include "nn/util.h"
#include "angelscript.h"
#include "lib/json.hpp"
#include "tas/Playback.hpp"
#include "Logger.hpp"
using json = nlohmann::json;
using Logger = lotuskit::Logger;

namespace lotuskit::script::globals {
    void trashPrintHookLimits() {
        Logger::logJson(json::object({
            {"inlineUsed", exl::hook::nx64::InlineEntryIndex},
            {"inlineMax", exl::hook::nx64::InlinePoolCount},
            {"hookUsed", exl::hook::nx64::HookEntryIndex},
            {"hookMax", exl::hook::nx64::HookMax}
        }), "/script/trashPrintHookLimits");
    }
    //void trashPrint(auto &msg) {
    //    const char* str = msg.c_str();
    //    svcOutputDebugString(str, strlen(str));
    //}
    void trashPrintInt(int num) {
        Logger::logJson(json::object({ {"oink", num} }), "/script/trashPrintInt");
    }
    void registerGlobals(AngelScript::asIScriptEngine* engine) {
        //engine->SetDefaultNamespace("Logger");
        //engine->RegisterGlobalFunction("void logText(string, string)", AngelScript::asFUNCTION(Logger::logText), AngelScript::asCALL_CDECL);
        s32 asErrno = engine->RegisterGlobalFunction("void trashPrintInt(int)", AngelScript::asFUNCTION(trashPrintInt), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );
        asErrno = engine->RegisterGlobalFunction("void trashPrintHookLimits()", AngelScript::asFUNCTION(trashPrintHookLimits), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );
        //asErrno = engine->RegisterGlobalFunction("void trashPrint(const string &in)", AngelScript::asFUNCTION(trashPrint), AngelScript::asCALL_CDECL); assert( asErrno >= 0 );

        engine->SetDefaultNamespace("tas");
        asErrno = engine->RegisterGlobalFunction(
            "void input(uint duration60=2, uint64 nextButtons=0, int nextLStickX=0, int nextLStickY=0, int nextRStickX=0, int nextRStickY=0)",
            AngelScript::asFUNCTION(lotuskit::tas::Playback::setCurrentInput),
            AngelScript::asCALL_CDECL
        ); assert( asErrno >= 0 );

    }
} // ns
