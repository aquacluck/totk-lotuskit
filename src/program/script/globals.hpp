#pragma once
#include "nn/util.h"
#include "angelscript.h"
#include "lib/json.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"
#include "Logger.hpp"
#include "TextWriter.hpp"
using json = nlohmann::json;
using Logger = lotuskit::Logger;

#include "heap/seadHeapMgr.h"
#include "thread/seadThread.h"
#include "syms_merged.hpp"


namespace lotuskit::script::globals {

    namespace sys {
        void heapInfo() {
            const auto rootheap0 = sead::HeapMgr::sRootHeaps[0];
            sead::Heap* h = rootheap0; // current/visiting entry
            sead::Heap* htmp = nullptr;
            u32 depth = 0;
            char printbuf[256];
            char leftpad[65] = "                                                                ";

            HEAPINFO_VISIT:
            // log the entry
            leftpad[depth*4] = '\0'; // assert depth <= 16
            nn::util::SNPrintf(printbuf, sizeof(printbuf), "%s%s(%p)", leftpad, h->getName().cstr(), h);
            Logger::logJson(json::object({
                {"msg", printbuf},
                //{"ptr", h}, {"depth", depth}, // TODO hex string? proper tree/ptr structure? split up printbuf
                {"size", h->getSize()},
                {"free", h->getFreeSize()}
            }), "/script/sys/heapInfo");
            leftpad[depth*4] = ' ';

            // keep descending into first child
            htmp = h->mChildren.front();
            if (htmp != nullptr) {
                depth++;
                h = htmp;
                goto HEAPINFO_VISIT;
            }

            // no children, proceed to siblings
            HEAPINFO_DO_NEXT:
            if (h->mParent == nullptr) {
                // assert h == rootheap0
                return; // ok
            }
            htmp = h->mParent->mChildren.next(h);
            if (htmp != nullptr) {
                h = htmp;
                goto HEAPINFO_VISIT; // advance through siblings
            }

            // no more siblings, ascend and continue advancing through the aunts and uncles
            depth--;
            h = h->mParent;
            goto HEAPINFO_DO_NEXT; // advance through siblings

            return; // unreachable
        }
        void hookLimits() {
            Logger::logJson(json::object({
                {"inlineUsed", exl::hook::nx64::InlineEntryIndex},
                {"inlineMax", exl::hook::nx64::InlinePoolCount},
                {"hookUsed", exl::hook::nx64::HookEntryIndex},
                {"hookMax", exl::hook::nx64::HookMax}
            }), "/script/sys/hookLimits");
            lotuskit::TextWriter::toastf(
                5*30, "[sys::hookLimits] inlineUsed: %d, inlineMax: %d, hookUsed: %d, hookMax: %d\n",
                exl::hook::nx64::InlineEntryIndex, exl::hook::nx64::InlinePoolCount, exl::hook::nx64::HookEntryIndex, exl::hook::nx64::HookMax
            );
        }
        void memSearch(u64 needle) {
            // TODO better ptr+hex conversions for AS calls + json?
            const auto rootheap0 = sead::HeapMgr::sRootHeaps[0];
            //Logger::main->logf(NS_DEFAULT_TEXT, "\"searching for %p in root heap %p\"", needle_ptr, rootheap0);
            void** haystackPtr = (void**)(rootheap0->getStartAddress());
            void** haystackEnd = (void**)(rootheap0->getEndAddress());
            while (haystackPtr < haystackEnd) {
                //if ((u64)haystackPtr % 0x800000 == 0) { Logger::main->logf(NS_DEFAULT_TEXT, "\"%p \"", haystackPtr); } // progress
                if ((u64)(*haystackPtr) == needle) {
                    Logger::logJson(json::object({
                        {"match", (u64)haystackPtr},
                        {"needle", needle}
                    }), "/script/sys/memSearch");
                    lotuskit::TextWriter::toastf(5*30, "[match] found %p at %p\n", needle, haystackPtr);
                }
                haystackPtr++;
            }
        }
        void threadInfo() {
            sead::ThreadMgr* mgr = *exl::util::pointer_path::FollowSafe<sead::ThreadMgr*, sym::sead::ThreadMgr::sInstance::offset>();
            sead::ThreadList* threads = &(mgr->mList);
            //output["main"] = json::object({ {"id", mgr->mMainThread->getId()}, {"name", mgr->mMainThread->getName().cstr()} });
            Logger::logJson(json::object({ {"id", mgr->mMainThread->getId()}, {"name", mgr->mMainThread->getName().cstr()}, {"isMainThread", true} }), "/script/sys/threadInfo");

            // XXX ScopedLock<CriticalSection> lock(getListCS());
            const auto end = threads->end();
            for (auto it = threads->begin(); it != end; ++it) {
                // they seem to always be running in practice?
                /*
                u32 state_ = (*it)->getState(); // SEAD_ENUM(State, cInitialized, cRunning, cQuitting, cTerminated, cReleased)
                char state = state_ == 0 ? 'i' : // initialized
                             state_ == 1 ? 'r' : // running
                             state_ == 2 ? 'q' : // quitting
                             state_ == 3 ? 't' : // terminated
                             state_ == 4 ? '_' : // released
                             '?';
                */
                auto name = (*it)->getName().cstr();
                auto tid = (*it)->getId();
                //FIXME cant log all at once -- stack or fakeheap exhausted? idk. TODO break off a sead::Heap and alloc more stuff through it
                //output[std::to_string(tid)] = json::object({ {"id", tid}, {"name", name}, {"state", state} });
                Logger::logJson(json::object({ {"id", tid}, {"name", name} }), "/script/sys/threadInfo");
            }
        }
        void suspendCtx() {
            AngelScript::asIScriptContext *ctx = AngelScript::asGetActiveContext();
            if (ctx != nullptr) { ctx->Suspend(); }
        }
    } // ns

    // ns Logger
    //void trashPrint(auto &msg) {
    //    const char* str = msg.c_str();
    //    svcOutputDebugString(str, strlen(str));
    //}
    //void trashPrintInt(int num) {
    //    Logger::logJson(json::object({ {"oink", num} }), "/script/trashPrintInt");
    //}

    void registerGlobals(AngelScript::asIScriptEngine* engine) {
        s32 asErrno = engine->RegisterGlobalFunction("void yield()", AngelScript::asFUNCTION(sys::suspendCtx), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

        //engine->SetDefaultNamespace("Logger");
        //engine->RegisterGlobalFunction("void logText(string, string)", AngelScript::asFUNCTION(Logger::logText), AngelScript::asCALL_CDECL);
        //s32 asErrno = engine->RegisterGlobalFunction("void trashPrintInt(int)", AngelScript::asFUNCTION(trashPrintInt), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void trashPrint(const string &in)", AngelScript::asFUNCTION(trashPrint), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

        engine->SetDefaultNamespace("sys");
        asErrno = engine->RegisterGlobalFunction("void heapInfo()", AngelScript::asFUNCTION(sys::heapInfo), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void hookLimits()", AngelScript::asFUNCTION(sys::hookLimits), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void memSearch(uint64)", AngelScript::asFUNCTION(sys::memSearch), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void threadInfo()", AngelScript::asFUNCTION(sys::threadInfo), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void suspendCtx()", AngelScript::asFUNCTION(sys::suspendCtx), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

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
