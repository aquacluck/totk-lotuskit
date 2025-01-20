#include "exlaunch.hpp"
#include "script/globals.hpp"

#include <nn/util.h>
#include <lib/json.hpp>
#include "tas/Playback.hpp"
#include "tas/Record.hpp"
#include "ActorWatcher.hpp"
#include "HexDump.hpp"
#include "Logger.hpp"
#include "TextWriter.hpp"
#include "util/actor.hpp"
#include "util/camera.hpp"
using json = nlohmann::json;
using Logger = lotuskit::Logger;
#include <math/seadVector.h>
#include <heap/seadHeapMgr.h>
#include <thread/seadThread.h>
#include <string>


namespace lotuskit::script::globals {
    namespace ResidentActors {
        ::engine::actor::ActorBase* Player = nullptr;
        ::engine::actor::ActorBase* PlayerCamera = nullptr;
        ::engine::actor::ActorBase* EventCamera = nullptr;
    } // ns


    // Begin AS helpers+impls


    namespace sys {
        u64 mainOffset() { return exl::util::GetMainModuleInfo().m_Total.m_Start; }
        u32 totkVersion() { return TOTK_VERSION; }
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
                {"inlineUsed", exl::hook::nx64::s_InlineEntryIndex},
                {"inlineMax", exl::hook::nx64::InlinePoolCount},
                {"hookUsed", exl::hook::nx64::s_HookEntryIndex},
                {"hookMax", exl::hook::nx64::HookMax}
            }), "/script/sys/hookLimits");
            lotuskit::TextWriter::toastf(
                5*30, "[sys::hookLimits] inlineUsed: %d, inlineMax: %d, hookUsed: %d, hookMax: %d\n",
                exl::hook::nx64::s_InlineEntryIndex, exl::hook::nx64::InlinePoolCount, exl::hook::nx64::s_HookEntryIndex, exl::hook::nx64::HookMax
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
            sead::ThreadMgr* mgr = *EXL_SYM_RESOLVE<sead::ThreadMgr**>("sead::ThreadMgr::sInstance");
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
        void debugLog(const std::string& msg) {
            svcOutputDebugString(msg.c_str(), msg.size());
        }
    } // ns

    void textwriter_as_print(size_t drawList_i, const std::string& msg) { lotuskit::TextWriter::printf(drawList_i, msg.c_str()); }
    void textwriter_as_toast(u32 ttlFrames, const std::string& msg) { lotuskit::TextWriter::toastf(ttlFrames, msg.c_str()); }

    sead::Vector3f actor_pos_get(::engine::actor::ActorBase* actor) { return actor->mPosition; }
    float actor_pos_get_x(::engine::actor::ActorBase* actor) { return actor->mPosition.x; }
    float actor_pos_get_y(::engine::actor::ActorBase* actor) { return actor->mPosition.y; }
    float actor_pos_get_z(::engine::actor::ActorBase* actor) { return actor->mPosition.z; }
    void actor_pos_set(::engine::actor::ActorBase* actor, const sead::Vector3f &pos) { lotuskit::util::actor::setPos(actor, pos); }
    void actor_pos_set_x(::engine::actor::ActorBase* actor, float x) { lotuskit::util::actor::setPosXYZ(actor, x, actor->mPosition.y, actor->mPosition.z); }
    void actor_pos_set_y(::engine::actor::ActorBase* actor, float y) { lotuskit::util::actor::setPosXYZ(actor, actor->mPosition.x, y, actor->mPosition.z); }
    void actor_pos_set_z(::engine::actor::ActorBase* actor, float z) { lotuskit::util::actor::setPosXYZ(actor, actor->mPosition.x, actor->mPosition.y, z); }


    // Begin AS engine registrations


    void registerBaseTypes(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;
        engine->SetDefaultNamespace(""); // root
        asErrno = engine->RegisterTypedef("u8", "uint8"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("u16", "uint16"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("u32", "uint"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("u64", "uint64"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("s8", "int8"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("s16", "int16"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("s32", "int"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("s64", "int64"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("ptrdiff_t", "int64"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("ptr_t", "uint64"); assert(asErrno >= 0); // AS has no raw pointers, but we often need to deal with their values
        asErrno = engine->RegisterTypedef("size_t", "uint64"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("index_t", "uint64"); assert(asErrno >= 0); // more appropriately named size_t
        asErrno = engine->RegisterTypedef("flagset_t", "uint64"); assert(asErrno >= 0);
        asErrno = engine->RegisterTypedef("f16", "uint16"); assert(asErrno >= 0); // AS has no half floats, just to preserve context/compat
        asErrno = engine->RegisterTypedef("f16_fake", "uint16"); assert(asErrno >= 0);
    }

    // asOBJ_APP_CLASS_CAK=Construct Assign Kopy
    void Vector2fConstructor(sead::Vector2f *self) { new(self) sead::Vector2f(); }
    void Vector2fAssignConstructor(float x, float y, sead::Vector2f *self) { new(self) sead::Vector2f(x,y); }
    void Vector2fKopyKonstructor(const sead::Vector2f &other, sead::Vector2f *self) { new(self) sead::Vector2f(other); }
    sead::Vector2f Vector2fAdd(const sead::Vector2f& a, const sead::Vector2f& b) { return a+b; }
    sead::Vector2f Vector2fSub(const sead::Vector2f& a, const sead::Vector2f& b) { return a-b; }
    sead::Vector2f Vector2fMul(const sead::Vector2f& a, float b) { return a*b; }
    sead::Vector2f Vector2fMul_r(float b, const sead::Vector2f& a) { return a*b; }
    sead::Vector2f Vector2fDiv(const sead::Vector2f& a, float b) { return a/b; }

    void Vector3fConstructor(sead::Vector3f *self) { new(self) sead::Vector3f(); }
    void Vector3fAssignConstructor(float x, float y, float z, sead::Vector3f *self) { new(self) sead::Vector3f(x,y,z); }
    void Vector3fKopyKonstructor(const sead::Vector3f &other, sead::Vector3f *self) { new(self) sead::Vector3f(other); }
    sead::Vector3f Vector3fAdd(const sead::Vector3f& a, const sead::Vector3f& b) { return a+b; }
    sead::Vector3f Vector3fSub(const sead::Vector3f& a, const sead::Vector3f& b) { return a-b; }
    sead::Vector3f Vector3fMul(const sead::Vector3f& a, float b) { return a*b; }
    sead::Vector3f Vector3fMul_r(float b, const sead::Vector3f& a) { return a*b; }
    sead::Vector3f Vector3fDiv(const sead::Vector3f& a, float b) { return a/b; }

    void registerContainers(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;
        engine->SetDefaultNamespace(""); // root

        // Vector2f
        asErrno = engine->RegisterObjectType("Vector2f", sizeof(sead::Vector2f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Vector2f", "float x", asOFFSET(sead::Vector2f, x)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Vector2f", "float y", asOFFSET(sead::Vector2f, y)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Vector2f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Vector2fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Vector2f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float, float y = 0)", AngelScript::asFUNCTION(Vector2fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Vector2f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Vector2f &in)", AngelScript::asFUNCTION(Vector2fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "float dot() const", AngelScript::asMETHOD(sead::Vector2f, dot), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "float length() const", AngelScript::asMETHOD(sead::Vector2f, length), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );

        // Vector2f operators
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f &opAddAssign(const Vector2f &in)", AngelScript::asMETHODPR(sead::Vector2f, operator+=, (const sead::Vector2f &), sead::Vector2f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f &opSubAssign(const Vector2f &in)", AngelScript::asMETHODPR(sead::Vector2f, operator-=, (const sead::Vector2f &), sead::Vector2f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f &opMulAssign(float)", AngelScript::asMETHODPR(sead::Vector2f, operator*=, (float), sead::Vector2f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f &opDivAssign(float)", AngelScript::asMETHODPR(sead::Vector2f, operator/=, (float), sead::Vector2f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "bool opEquals(const Vector2f &in) const", AngelScript::asMETHOD(sead::Vector2f, operator==), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f opAdd(const Vector2f &in) const", AngelScript::asFUNCTIONPR(Vector2fAdd, (const sead::Vector2f&, const sead::Vector2f&), sead::Vector2f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f opSub(const Vector2f &in) const", AngelScript::asFUNCTIONPR(Vector2fSub, (const sead::Vector2f&, const sead::Vector2f&), sead::Vector2f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f opMul(float) const", AngelScript::asFUNCTIONPR(Vector2fMul, (const sead::Vector2f&, float), sead::Vector2f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f opMul_r(float) const", AngelScript::asFUNCTIONPR(Vector2fMul_r, (float, const sead::Vector2f&), sead::Vector2f), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector2f", "Vector2f opDiv(float) const", AngelScript::asFUNCTIONPR(Vector2fDiv, (const sead::Vector2f&, float), sead::Vector2f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );

        // Vector3f
        asErrno = engine->RegisterObjectType("Vector3f", sizeof(sead::Vector3f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Vector3f", "float x", asOFFSET(sead::Vector3f, x)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Vector3f", "float y", asOFFSET(sead::Vector3f, y)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Vector3f", "float z", asOFFSET(sead::Vector3f, z)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Vector3f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Vector3fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Vector3f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float, float y = 0, float z = 0)", AngelScript::asFUNCTION(Vector3fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Vector3f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Vector3f &in)", AngelScript::asFUNCTION(Vector3fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "float dot() const", AngelScript::asMETHOD(sead::Vector3f, dot), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "float length() const", AngelScript::asMETHOD(sead::Vector3f, length), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        // TODO cross, normalize, rotate?

        // Vector3f operators
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f &opAddAssign(const Vector3f &in)", AngelScript::asMETHODPR(sead::Vector3f, operator+=, (const sead::Vector3f &), sead::Vector3f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f &opSubAssign(const Vector3f &in)", AngelScript::asMETHODPR(sead::Vector3f, operator-=, (const sead::Vector3f &), sead::Vector3f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f &opMulAssign(float)", AngelScript::asMETHODPR(sead::Vector3f, operator*=, (float), sead::Vector3f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f &opDivAssign(float)", AngelScript::asMETHODPR(sead::Vector3f, operator/=, (float), sead::Vector3f&), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "bool opEquals(const Vector3f &in) const", AngelScript::asMETHOD(sead::Vector3f, operator==), AngelScript::asCALL_THISCALL); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f opAdd(const Vector3f &in) const", AngelScript::asFUNCTIONPR(Vector3fAdd, (const sead::Vector3f&, const sead::Vector3f&), sead::Vector3f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f opSub(const Vector3f &in) const", AngelScript::asFUNCTIONPR(Vector3fSub, (const sead::Vector3f&, const sead::Vector3f&), sead::Vector3f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f opMul(float) const", AngelScript::asFUNCTIONPR(Vector3fMul, (const sead::Vector3f&, float), sead::Vector3f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f opMul_r(float) const", AngelScript::asFUNCTIONPR(Vector3fMul_r, (float, const sead::Vector3f&), sead::Vector3f), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Vector3f", "Vector3f opDiv(float) const", AngelScript::asFUNCTIONPR(Vector3fDiv, (const sead::Vector3f&, float), sead::Vector3f), AngelScript::asCALL_CDECL_OBJFIRST); assert( asErrno >= 0 );

        // TODO Vector4f, Quatf, doubles, ints?

        // TODO Matrix33f, Matrix43f, Matrix44f, Matrix22f, BoundBox3f
    }

    void registerUtil(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;

        engine->SetDefaultNamespace(""); // root
        asErrno = engine->RegisterGlobalFunction("void yield()", AngelScript::asFUNCTION(sys::suspendCtx), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

        engine->SetDefaultNamespace("sys"); /// {
            asErrno = engine->RegisterGlobalFunction("void heapInfo()", AngelScript::asFUNCTION(sys::heapInfo), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void hookLimits()", AngelScript::asFUNCTION(sys::hookLimits), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void memSearch(ptr_t)", AngelScript::asFUNCTION(sys::memSearch), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void threadInfo()", AngelScript::asFUNCTION(sys::threadInfo), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void suspendCtx()", AngelScript::asFUNCTION(sys::suspendCtx), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("ptr_t mainOffset()", AngelScript::asFUNCTION(sys::mainOffset), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("u32 totkVersion()", AngelScript::asFUNCTION(sys::totkVersion), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void debugLog(const string &in)", AngelScript::asFUNCTION(sys::debugLog), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("Logger"); /// {
            engine->RegisterGlobalFunction("void logText(const string &in, const string &in = \"/Logger\", bool = false, bool = true )", AngelScript::asFUNCTION(Logger::logText), AngelScript::asCALL_CDECL);
        /// }

        engine->SetDefaultNamespace("HexDump"); /// {
            asErrno = engine->RegisterGlobalFunction("void clearSlot(index_t)", AngelScript::asFUNCTION(lotuskit::HexDump::clearSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void pauseSlot(index_t)", AngelScript::asFUNCTION(lotuskit::HexDump::pauseSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAbsolute(index_t, ptr_t, u32, u32)", AngelScript::asFUNCTION(lotuskit::HexDump::assignSlotAbsolute), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("TextWriter"); /// {
            // TODO do AS variadic stuff to hopefully just forward the args to normal snprintf/whatever -- format("{} {}", 1, 2) also exists for now
            asErrno = engine->RegisterGlobalFunction("void print(index_t, const string &in)", AngelScript::asFUNCTION(textwriter_as_print), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void toast(u32, const string &in)", AngelScript::asFUNCTION(textwriter_as_toast), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("camera"); /// {
            // camera hacks
            asErrno = engine->RegisterGlobalFunction("void toggleFreeze()", AngelScript::asFUNCTION(lotuskit::util::camera::toggleFreeze), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void freeze(float, float, float, double, double, double, double, double, double)", AngelScript::asFUNCTION(lotuskit::util::camera::freeze333), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void log()", AngelScript::asFUNCTION(lotuskit::util::camera::log), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }
    }

    void registerTAS(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;
        // define nx-TAS buttons: KEY_ZL;KEY_X -> KEY_ZL|KEY_X
        engine->SetDefaultNamespace(""); /// root {
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
        /// }

        engine->SetDefaultNamespace("tas"); /// {
            // TODO tas::yieldInput(frameCount) -- consume time in tas scheduling but do not override input
            asErrno = engine->RegisterGlobalFunction(
                "void input(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0)",
                AngelScript::asFUNCTION(lotuskit::tas::Playback::setCurrentInput),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void toggleDump()",
                AngelScript::asFUNCTION(lotuskit::tas::Record::trashToggleDump),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
        /// }
    }

    void registerActorSystem(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;

        engine->SetDefaultNamespace(""); /// root {
            //asErrno = engine->RegisterObjectType("PreActor", 0, AngelScript::asOBJ_REF | AngelScript::asOBJ_NOCOUNT); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectType("ActorBase", 0, AngelScript::asOBJ_REF | AngelScript::asOBJ_NOCOUNT); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "Vector3f get_pos() property", AngelScript::asFUNCTION(actor_pos_get), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "float get_pos_x() property", AngelScript::asFUNCTION(actor_pos_get_x), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "float get_pos_y() property", AngelScript::asFUNCTION(actor_pos_get_y), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "float get_pos_z() property", AngelScript::asFUNCTION(actor_pos_get_z), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void set_pos(const Vector3f &in) property", AngelScript::asFUNCTION(actor_pos_set), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void set_pos_x(float) property", AngelScript::asFUNCTION(actor_pos_set_x), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void set_pos_y(float) property", AngelScript::asFUNCTION(actor_pos_set_y), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void set_pos_z(float) property", AngelScript::asFUNCTION(actor_pos_set_z), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setPos(const Vector3f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::setPos), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setPos(float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::setPosXYZ), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setRot(float, float, float, float, float, float, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::setRot9), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setPosRot(float, float, float, float, float, float, float, float, float, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::setPosRot39), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "RigidBody@ getMainRigidBody()", AngelScript::asFUNCTION(lotuskit::util::actor::getMainRigidBody), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            //engine->SetDefaultNamespace("ResidentActors");
            asErrno = engine->RegisterGlobalProperty("ActorBase@ Player", &ResidentActors::Player); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ PlayerCamera", &ResidentActors::PlayerCamera); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ EventCamera", &ResidentActors::EventCamera); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("actor"); /// {
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimple), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimpleXYZ), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in, const Vector3f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimplePos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatch), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatchXYZ), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in, const Vector3f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatchPos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("ActorWatcher"); /// {
            // slot mgmt
            asErrno = engine->RegisterGlobalFunction("void clearSlot(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::clearSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void pauseSlot(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::pauseSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlot(index_t, ActorBase@)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlot(index_t, ptr_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotPreActor(index_t, ptr_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotPreActor), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            //asErrno = engine->RegisterGlobalFunction("void assignSlotPreActor(index_t, PreActor@)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotPreActor), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAwaitRecall(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotAwaitRecall), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAwaitSpawn(index_t, string &in)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotAwaitSpawn), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

            // config
            asErrno = engine->RegisterGlobalFunction("void doTextWriter(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doTextWriter), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doWsLog(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doWsLog), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

            asErrno = engine->RegisterGlobalFunction("void doDrawAABB(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawAABB), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doDrawPos(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawPos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doDrawVel(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawVel), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doDrawAngVel(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawAngVel), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

            asErrno = engine->RegisterGlobalFunction("void doDrawModelPos(index_t, bool)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawModelPos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

            asErrno = engine->RegisterGlobalFunction("void doDrawRigidBodyAABB(index_t, flagset_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawRigidBodyAABB), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doDrawRigidBodyPos(index_t, flagset_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawRigidBodyPos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doDrawRigidBodyPosPast(index_t, flagset_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawRigidBodyPosPast), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void doDrawRigidBodyPosFuture(index_t, flagset_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::doDrawRigidBodyPosFuture), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

            // slot access
            asErrno = engine->RegisterGlobalFunction("ActorBase@ get(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::getSlotActor), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("ActorBase@ getSlotActor(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::getSlotActor), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ actor0", &(lotuskit::ActorWatcher::slots[0].actor)); assert(asErrno >= 0); // TODO loop these calls for MAX_WATCHER_SLOTS?
            asErrno = engine->RegisterGlobalProperty("ActorBase@ actor1", &(lotuskit::ActorWatcher::slots[1].actor)); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ actor2", &(lotuskit::ActorWatcher::slots[2].actor)); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ actor3", &(lotuskit::ActorWatcher::slots[3].actor)); assert(asErrno >= 0);
        /// }
    }

    void registerPhive(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;
        engine->SetDefaultNamespace(""); // root
        asErrno = engine->RegisterObjectType("RigidBody", 0, AngelScript::asOBJ_REF | AngelScript::asOBJ_NOCOUNT); assert(asErrno >= 0);
        // RigidBodyBase+RigidBodyEntity methods
        asErrno = engine->RegisterObjectMethod("RigidBody", "void setVel(const Vector3f &in)", AngelScript::asMETHOD(phive::RigidBodyBase, requestSetLinearVelocity), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void setVel(float, float, float)", AngelScript::asMETHOD(phive::RigidBodyBase, requestSetLinearVelocityXYZ), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void applyImpulse(const Vector3f &in)", AngelScript::asMETHOD(phive::RigidBodyBase, applyLinearImpulse), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void applyImpulse(float, float, float)", AngelScript::asMETHOD(phive::RigidBodyBase, applyLinearImpulseXYZ), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
    }

    void registerGlobals(AngelScript::asIScriptEngine* engine) {
        registerBaseTypes(engine);
        registerContainers(engine);
        registerUtil(engine);
        registerTAS(engine);
        registerPhive(engine);
        registerActorSystem(engine);
    }

} // ns
