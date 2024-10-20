#pragma once
#include "nn/util.h"
#include "angelscript.h"
#include "lib/json.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"
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
        asErrno = engine->RegisterGlobalFunction(
            "void toggleDump()",
            AngelScript::asFUNCTION(lotuskit::tas::Record::trashToggleDump),
            AngelScript::asCALL_CDECL
        ); assert( asErrno >= 0 );

        // define nx-TAS buttons: KEY_ZL;KEY_X -> KEY_ZL|KEY_X
        engine->SetDefaultNamespace("");
        asErrno = engine->RegisterEnum("nxTASButton"); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "NONE", 0); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_A", (1 << 0)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_B", (1 << 1)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_X", (1 << 2)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_Y", (1 << 3)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_L", (1 << 6)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_R", (1 << 7)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_ZL", (1 << 8)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_ZR", (1 << 9)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_PLUS", (1 << 10)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_MINUS", (1 << 11)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_DLEFT", (1 << 12)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_DUP", (1 << 13)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_DRIGHT", (1 << 14)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_DDOWN", (1 << 15)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_LSTICK", (1 << 4)); assert(asErrno >= 0);
        asErrno = engine->RegisterEnumValue("nxTASButton", "KEY_RSTICK", (1 << 5)); assert(asErrno >= 0);

    }
} // ns
