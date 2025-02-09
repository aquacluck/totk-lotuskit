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
#include "PrimitiveDrawer.hpp"
#include "server/WebSocket.hpp"
#include "util/actor.hpp"
#include "util/camera.hpp"
#include "util/color.hpp"
#include "util/event.hpp"
#include "util/player.hpp"
#include "util/pause.hpp"
using json = nlohmann::json;
using Logger = lotuskit::Logger;
#include <gfx/seadColor.h>
#include <gfx/seadPrimitiveRenderer.h>
#include <math/seadVector.h>
#include <heap/seadHeapMgr.h>
#include <thread/seadThread.h>
#include <cmath>
#include <string>


namespace lotuskit::script::globals {
    namespace ResidentActors {
        ::engine::actor::ActorBase* Player = nullptr;
        ::engine::actor::ActorBase* PlayerCamera = nullptr;
        ::engine::actor::ActorBase* EventCamera = nullptr;
    } // ns
    namespace Vector2fStatic {
        // XXX AS RegisterGlobalProperty won't let these be const?
        sead::Vector2f STICK_ZERO(0, 0);
        sead::Vector2f STICK_UP_MAX(0, 32767);
        sead::Vector2f STICK_UP_DIR(0, 1);
        sead::Vector2f STICK_DOWN_MAX(0, -32768);
        sead::Vector2f STICK_DOWN_DIR(0, -1);
        sead::Vector2f STICK_LEFT_MAX(-32768, 0);
        sead::Vector2f STICK_LEFT_DIR(-1, 0);
        sead::Vector2f STICK_RIGHT_MAX(32767, 0);
        sead::Vector2f STICK_RIGHT_DIR(1, 0);
    } // ns
    namespace Vector3fStatic {
        sead::Vector3f  ZERO( 0,  0,  0);
        sead::Vector3f   ONE( 1,  1,  1);
        sead::Vector3f   NEG(-1, -1, -1);
        sead::Vector3f    UP( 0,  1,  0);
        sead::Vector3f  DOWN( 0, -1,  0);
        sead::Vector3f  EAST( 1,  0,  0);
        sead::Vector3f  WEST(-1,  0,  0);
        sead::Vector3f NORTH( 0,  0, -1);
        sead::Vector3f SOUTH( 0,  0,  1);
        sead::Vector3f NORTHEAST( M_SQRT1_2, 0, -M_SQRT1_2);
        sead::Vector3f NORTHWEST(-M_SQRT1_2, 0, -M_SQRT1_2);
        sead::Vector3f SOUTHEAST( M_SQRT1_2, 0,  M_SQRT1_2);
        sead::Vector3f SOUTHWEST(-M_SQRT1_2, 0,  M_SQRT1_2);
    } // ns
    namespace Matrix22fStatic {
        sead::Matrix22f IDENTITY(1,0, 0,1);
        sead::Matrix22f ZERO(0,0, 0,0);
        sead::Matrix22f ONE(1,1, 1,1);
        sead::Matrix22f NEG(-1,-1, -1,-1);
    } // ns
    namespace Matrix33fStatic {
        sead::Matrix33f IDENTITY(1,0,0, 0,1,0,   0,0,1);
        sead::Matrix33f ZERO(0, 0, 0,   0,0,0,   0,0,0);
        sead::Matrix33f ONE( 1, 1, 1,   1,1,1,   1,1,1);
        sead::Matrix33f NEG(-1,-1,-1,  -1,-1,-1, -1,-1,-1);

        sead::Matrix33f NORTH(-1,0, 0,  0,1,0,   0,0,-1);
        sead::Matrix33f SOUTH( 1,0, 0,  0,1,0,   0,0, 1); // identity
        sead::Matrix33f EAST(  0,0, 1,  0,1,0,  -1,0, 0);
        sead::Matrix33f WEST(  0,0,-1,  0,1,0,   1,0, 0);

        sead::Matrix33f NORTHEAST(-M_SQRT1_2,0, M_SQRT1_2,   0,1,0,  -M_SQRT1_2,0,-M_SQRT1_2);
        sead::Matrix33f NORTHWEST(-M_SQRT1_2,0,-M_SQRT1_2,   0,1,0,   M_SQRT1_2,0,-M_SQRT1_2);
        sead::Matrix33f SOUTHEAST( M_SQRT1_2,0, M_SQRT1_2,   0,1,0,  -M_SQRT1_2,0, M_SQRT1_2);
        sead::Matrix33f SOUTHWEST( M_SQRT1_2,0,-M_SQRT1_2,   0,1,0,   M_SQRT1_2,0, M_SQRT1_2);

        // facing up or down + specified roll
        sead::Matrix33f DOWN__UP_IS_EAST(  0, 1,0,   0,0,-1,  -1, 0,0);
        sead::Matrix33f DOWN__UP_IS_WEST(  0,-1,0,   0,0,-1,   1, 0,0);
        sead::Matrix33f DOWN__UP_IS_NORTH(-1, 0,0,   0,0,-1,   0,-1,0);
        sead::Matrix33f DOWN__UP_IS_SOUTH( 1, 0,0,   0,0,-1,   0, 1,0);
        sead::Matrix33f UP__UP_IS_EAST(    0, 1,0,   0,0, 1,   1, 0,0);
        sead::Matrix33f UP__UP_IS_WEST(    0,-1,0,   0,0, 1,  -1, 0,0);
        sead::Matrix33f UP__UP_IS_NORTH(   1, 0,0,   0,0, 1,   0,-1,0);
        sead::Matrix33f UP__UP_IS_SOUTH(  -1, 0,0,   0,0, 1,   0, 1,0);
        // TODO up=nsew, facing nsewud etc rotations? prob simpler to apply quats tho

    } // ns
    namespace Matrix34fStatic {
        sead::Matrix34f ZERO(0, 0, 0,   0,0,0,   0,0,0, 0,0,0);
        sead::Matrix34f ONE( 1, 1, 1,   1,1,1,   1,1,1, 1,1,1);
        sead::Matrix34f NEG(-1,-1,-1,  -1,-1,-1, -1,-1,-1, -1,-1,-1);
    } // ns
    namespace Matrix44fStatic {
        sead::Matrix44f IDENTITY(1,0,0,0,  0,1,0,0,   0,0,1,0, 0,0,0,1);
        sead::Matrix44f ZERO(0, 0, 0, 0,   0,0,0,0,   0,0,0,0, 0,0,0,0);
        sead::Matrix44f ONE( 1, 1, 1, 1,   1,1,1,1,   1,1,1,1, 1,1,1,1);
        sead::Matrix44f NEG(-1,-1,-1,-1,  -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1);
    } // ns


    // Begin AS helpers+impls


    namespace sys {
        u64 mainOffset() { return exl::util::GetMainModuleInfo().m_Total.m_Start; }
        u32 totkVersion() { return TOTK_VERSION; }
        u64 tick() { return svcGetSystemTick(); }
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
            char needleStr[24];
            nn::util::SNPrintf(needleStr, sizeof(needleStr), "%p", needle); // XXX trash conversions idk
            sead::HeapMgr* heapMgr = *EXL_SYM_RESOLVE<sead::HeapMgr**>("_ZN4sead7HeapMgr12sInstancePtrE");
            const auto rootheap0 = sead::HeapMgr::sRootHeaps[0];
            //Logger::main->logf(NS_DEFAULT_TEXT, "\"searching for %p in root heap %p\"", needle_ptr, rootheap0);
            void** haystackPtr = (void**)(rootheap0->getStartAddress());
            void** haystackEnd = (void**)(rootheap0->getEndAddress());
            while (haystackPtr < haystackEnd) {
                //if ((u64)haystackPtr % 0x800000 == 0) { Logger::main->logf(NS_DEFAULT_TEXT, "\"%p \"", haystackPtr); } // progress
                if ((u64)(*haystackPtr) == needle) {
                    char matchStr[24];
                    nn::util::SNPrintf(matchStr, sizeof(matchStr), "%p", haystackPtr);

                    const char* nullheap = "[no heap?]";
                    char heapAddrStr[24];
                    sead::Heap* heap = heapMgr->findContainHeap(haystackPtr);
                    nn::util::SNPrintf(heapAddrStr, sizeof(heapAddrStr), "%p", heap);

                    Logger::logJson(json::object({
                        {"match", matchStr},
                        {"needle", needleStr},
                        {"heap", heapAddrStr},
                        {"heapName", heap == nullptr ? nullheap : heap->getName().cstr() }
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
        void wsListen() {
            lotuskit::server::WebSocket::isInternalReqListen = true;
        }
    } // ns

    void textwriter_as_print(size_t drawList_i, const std::string& msg) { lotuskit::TextWriter::printf(drawList_i, msg.c_str()); }
    void textwriter_as_toast(u32 ttlFrames, const std::string& msg) { lotuskit::TextWriter::toastf(ttlFrames, msg.c_str()); }

    void primq_wirecube(size_t drawList_i, const sead::BoundBox3f& box, const sead::Color4f& color) { lotuskit::PrimitiveDrawer::drawWireCube(drawList_i, sead::PrimitiveDrawer::CubeArg(box, color)); }
    void primq_wirecube2(size_t drawList_i, const sead::Vector3f& pos, const sead::Vector3f& size, const sead::Color4f& color) { lotuskit::PrimitiveDrawer::drawWireCube(drawList_i, sead::PrimitiveDrawer::CubeArg(pos, size, color)); }

    sead::Vector3f actor_pos_get(::engine::actor::ActorBase* actor) { return actor->mPosition; }
    float actor_pos_get_x(::engine::actor::ActorBase* actor) { return actor->mPosition.x; }
    float actor_pos_get_y(::engine::actor::ActorBase* actor) { return actor->mPosition.y; }
    float actor_pos_get_z(::engine::actor::ActorBase* actor) { return actor->mPosition.z; }
    void actor_pos_set(::engine::actor::ActorBase* actor, const sead::Vector3f &pos) { lotuskit::util::actor::setPos(actor, pos); }
    void actor_pos_set_x(::engine::actor::ActorBase* actor, float x) { lotuskit::util::actor::setPosXYZ(actor, x, actor->mPosition.y, actor->mPosition.z); }
    void actor_pos_set_y(::engine::actor::ActorBase* actor, float y) { lotuskit::util::actor::setPosXYZ(actor, actor->mPosition.x, y, actor->mPosition.z); }
    void actor_pos_set_z(::engine::actor::ActorBase* actor, float z) { lotuskit::util::actor::setPosXYZ(actor, actor->mPosition.x, actor->mPosition.y, z); }
    sead::BoundBox3f actor_aabb_get(::engine::actor::ActorBase* actor) { return actor->mAABB; }
    std::string actor_name_get(::engine::actor::ActorBase* actor) { return actor->mActorName.cstr(); }
    sead::Matrix33f actor_rot_get(::engine::actor::ActorBase* actor) { return actor->mRotation; }

    void tas_input_vec2f_l(u32 duration, u64 nextButtons, const sead::Vector2f &nextLStick) {
        lotuskit::tas::Playback::setCurrentInput(duration, nextButtons, nextLStick.x, nextLStick.y, 0, 0);
    }
    void tas_input_vec2f_lr(u32 duration, u64 nextButtons, const sead::Vector2f &nextLStick, const sead::Vector2f &nextRStick) {
        lotuskit::tas::Playback::setCurrentInput(duration, nextButtons, nextLStick.x, nextLStick.y, nextRStick.x, nextRStick.y);
    }
    void tas_input_vec2f_0l(u32 duration, const sead::Vector2f &nextLStick) {
        lotuskit::tas::Playback::setCurrentInput(duration, 0, nextLStick.x, nextLStick.y, 0, 0);
    }
    void tas_input_vec2f_0lr(u32 duration, const sead::Vector2f &nextLStick, const sead::Vector2f &nextRStick) {
        lotuskit::tas::Playback::setCurrentInput(duration, 0, nextLStick.x, nextLStick.y, nextRStick.x, nextRStick.y);
    }
    void tas_inputor_vec2f_l(u32 duration, u64 nextButtons, const sead::Vector2f &nextLStick) {
        lotuskit::tas::Playback::setCurrentInputOr(duration, nextButtons, nextLStick.x, nextLStick.y, 0, 0);
    }
    void tas_inputor_vec2f_lr(u32 duration, u64 nextButtons, const sead::Vector2f &nextLStick, const sead::Vector2f &nextRStick) {
        lotuskit::tas::Playback::setCurrentInputOr(duration, nextButtons, nextLStick.x, nextLStick.y, nextRStick.x, nextRStick.y);
    }
    void tas_inputor_vec2f_0l(u32 duration, const sead::Vector2f &nextLStick) {
        lotuskit::tas::Playback::setCurrentInputOr(duration, 0, nextLStick.x, nextLStick.y, 0, 0);
    }
    void tas_inputor_vec2f_0lr(u32 duration, const sead::Vector2f &nextLStick, const sead::Vector2f &nextRStick) {
        lotuskit::tas::Playback::setCurrentInputOr(duration, 0, nextLStick.x, nextLStick.y, nextRStick.x, nextRStick.y);
    }
    void tas_inputxor_vec2f_l(u32 duration, u64 nextButtons, const sead::Vector2f &nextLStick) {
        lotuskit::tas::Playback::setCurrentInputXor(duration, nextButtons, nextLStick.x, nextLStick.y, 0, 0);
    }
    void tas_inputxor_vec2f_lr(u32 duration, u64 nextButtons, const sead::Vector2f &nextLStick, const sead::Vector2f &nextRStick) {
        lotuskit::tas::Playback::setCurrentInputXor(duration, nextButtons, nextLStick.x, nextLStick.y, nextRStick.x, nextRStick.y);
    }
    void tas_inputxor_vec2f_0l(u32 duration, const sead::Vector2f &nextLStick) {
        lotuskit::tas::Playback::setCurrentInputXor(duration, 0, nextLStick.x, nextLStick.y, 0, 0);
    }
    void tas_inputxor_vec2f_0lr(u32 duration, const sead::Vector2f &nextLStick, const sead::Vector2f &nextRStick) {
        lotuskit::tas::Playback::setCurrentInputXor(duration, 0, nextLStick.x, nextLStick.y, nextRStick.x, nextRStick.y);
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

    void Color4fConstructor(sead::Color4f *self) { new(self) sead::Color4f(); }
    void Color4fAssignConstructor(float r, float g, float b, float a, sead::Color4f *self) { new(self) sead::Color4f(r,g,b,a); }
    void Color4fKopyKonstructor(const sead::Color4f &other, sead::Color4f *self) { new(self) sead::Color4f(other); }

    void Matrix22fConstructor(sead::Matrix22f *self) { new(self) sead::Matrix22f(); }
    void Matrix22fAssignConstructor(float a0, float a1, float a2, float a3, sead::Matrix22f *self) { new(self) sead::Matrix22f(a0,a1,a2,a3); }
    void Matrix22fKopyKonstructor(const sead::Matrix22f &other, sead::Matrix22f *self) { new(self) sead::Matrix22f(other); }

    void Matrix33fConstructor(sead::Matrix33f *self) { new(self) sead::Matrix33f(); }
    void Matrix33fAssignConstructor(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, sead::Matrix33f *self) { new(self) sead::Matrix33f(a0,a1,a2,a3,a4,a5,a6,a7,a8); }
    void Matrix33fKopyKonstructor(const sead::Matrix33f &other, sead::Matrix33f *self) { new(self) sead::Matrix33f(other); }

    void Matrix34fConstructor(sead::Matrix34f *self) { new(self) sead::Matrix34f(); }
    void Matrix34fAssignConstructor(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, sead::Matrix34f *self) { new(self) sead::Matrix34f(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11); }
    void Matrix34fKopyKonstructor(const sead::Matrix34f &other, sead::Matrix34f *self) { new(self) sead::Matrix34f(other); }

    sead::Vector3f matrix34f_pos_get(const sead::Matrix34f &m) { return sead::Vector3f{m.a[3], m.a[7], m.a[11]}; }
    void matrix34f_pos_set(sead::Matrix34f &m, const sead::Vector3f &pos) { m.a[3] = pos.x; m.a[7] = pos.y; m.a[11] = pos.z; }
    sead::Matrix33f matrix34f_rot_get(const sead::Matrix34f &m) {
        sead::Matrix33f rot;
        rot.m[0][0] = m.a[0]; rot.m[0][1] = m.a[1]; rot.m[0][2] = m.a[2];
        rot.m[1][0] = m.a[4]; rot.m[1][1] = m.a[5]; rot.m[1][2] = m.a[6];
        rot.m[2][0] = m.a[8]; rot.m[2][1] = m.a[9]; rot.m[2][2] = m.a[10];
        return rot;
    }
    void matrix34f_rot_set(sead::Matrix34f &m, const sead::Matrix33f &rot) {
        m.a[0] = rot.m[0][0]; m.a[1] = rot.m[0][1]; m.a[ 2] = rot.m[0][2];
        m.a[4] = rot.m[1][0]; m.a[5] = rot.m[1][1]; m.a[ 6] = rot.m[1][2];
        m.a[8] = rot.m[2][0]; m.a[9] = rot.m[2][1]; m.a[10] = rot.m[2][2];
    }
    void matrix34f_posrot_set(sead::Matrix34f &m, const sead::Vector3f &pos, const sead::Matrix33f &rot) {
        m.a[3] = pos.x; m.a[7] = pos.y; m.a[11] = pos.z;
        m.a[0] = rot.m[0][0]; m.a[1] = rot.m[0][1]; m.a[ 2] = rot.m[0][2];
        m.a[4] = rot.m[1][0]; m.a[5] = rot.m[1][1]; m.a[ 6] = rot.m[1][2];
        m.a[8] = rot.m[2][0]; m.a[9] = rot.m[2][1]; m.a[10] = rot.m[2][2];
    }

    void Matrix44fConstructor(sead::Matrix44f *self) { new(self) sead::Matrix44f(); }
    void Matrix44fAssignConstructor(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13, float a14, float a15, sead::Matrix44f *self) { new(self) sead::Matrix44f(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15); }
    void Matrix44fKopyKonstructor(const sead::Matrix44f &other, sead::Matrix44f *self) { new(self) sead::Matrix44f(other); }

    void BoundBox2fConstructor(sead::BoundBox2f *self) { new(self) sead::BoundBox2f(); }
    void BoundBox2fAssignConstructor(float min_x, float min_y, float max_x, float max_y, sead::BoundBox2f *self) { new(self) sead::BoundBox2f(min_x, min_y, max_x, max_y); }
    void BoundBox2fAssignConstructorVecVec(const sead::Vector2f& min, const sead::Vector2f& max, sead::BoundBox2f *self) { new(self) sead::BoundBox2f(min, max); }
    void BoundBox2fKopyKonstructor(const sead::BoundBox2f &other, sead::BoundBox2f *self) { new(self) sead::BoundBox2f(other); }

    void BoundBox3fConstructor(sead::BoundBox3f *self) { new(self) sead::BoundBox3f(); }
    void BoundBox3fAssignConstructor(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z, sead::BoundBox3f *self) { new(self) sead::BoundBox3f(min_x, min_y, min_z, max_x, max_y, max_z); }
    void BoundBox3fAssignConstructorVecVec(const sead::Vector3f& min, const sead::Vector3f& max, sead::BoundBox3f *self) { new(self) sead::BoundBox3f(min, max); }
    void BoundBox3fKopyKonstructor(const sead::BoundBox3f &other, sead::BoundBox3f *self) { new(self) sead::BoundBox3f(other); }


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

        // Color4f
        asErrno = engine->RegisterObjectType("Color4f", sizeof(sead::Color4f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Color4f", "float r", asOFFSET(sead::Color4f, r)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Color4f", "float g", asOFFSET(sead::Color4f, g)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Color4f", "float b", asOFFSET(sead::Color4f, b)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Color4f", "float a", asOFFSET(sead::Color4f, a)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Color4f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Color4fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Color4f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float, float g = 0, float b = 0, float a = 1)", AngelScript::asFUNCTION(Color4fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Color4f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Color4f &in)", AngelScript::asFUNCTION(Color4fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectMethod("Color4f", "float get_h() property", AngelScript::asFUNCTION(lotuskit::util::color::hue_from_color), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("Color4f", "float get_s() property", AngelScript::asFUNCTION(lotuskit::util::color::sat_from_color), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("Color4f", "float get_v() property", AngelScript::asFUNCTION(lotuskit::util::color::val_from_color), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        engine->SetDefaultNamespace("Color4f"); // HSV factories+static
        asErrno = engine->RegisterGlobalFunction("Color4f from_hsva(float h=1, float s=1, float v=1, float a=1)", AngelScript::asFUNCTION(lotuskit::util::color::color_from_hsva), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        engine->SetDefaultNamespace(""); // root

        // TODO Quatf/Quatd, Vector3d, Vector2i?

        asErrno = engine->RegisterObjectType("Matrix22f", sizeof(sead::Matrix22f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Matrix22f", "float a0", asOFFSET(sead::Matrix22f, a[0])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix22f", "float a1", asOFFSET(sead::Matrix22f, a[1])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix22f", "float a2", asOFFSET(sead::Matrix22f, a[2])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix22f", "float a3", asOFFSET(sead::Matrix22f, a[3])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Matrix22f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Matrix22fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix22f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float a0, float a1=0, float a2=0, float a3=0, float a4=0, float a5=0, float a6=0, float a7=0, float a8=0)", AngelScript::asFUNCTION(Matrix22fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix22f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Matrix22f &in)", AngelScript::asFUNCTION(Matrix22fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );

        asErrno = engine->RegisterObjectType("Matrix33f", sizeof(sead::Matrix33f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a0", asOFFSET(sead::Matrix33f, a[0])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a1", asOFFSET(sead::Matrix33f, a[1])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a2", asOFFSET(sead::Matrix33f, a[2])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a3", asOFFSET(sead::Matrix33f, a[3])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a4", asOFFSET(sead::Matrix33f, a[4])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a5", asOFFSET(sead::Matrix33f, a[5])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a6", asOFFSET(sead::Matrix33f, a[6])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a7", asOFFSET(sead::Matrix33f, a[7])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix33f", "float a8", asOFFSET(sead::Matrix33f, a[8])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Matrix33f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Matrix33fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix33f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float a0, float a1=0, float a2=0, float a3=0, float a4=0, float a5=0, float a6=0, float a7=0, float a8=0)", AngelScript::asFUNCTION(Matrix33fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix33f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Matrix33f &in)", AngelScript::asFUNCTION(Matrix33fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );

        asErrno = engine->RegisterObjectType("Matrix34f", sizeof(sead::Matrix34f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a0", asOFFSET(sead::Matrix34f, a[0])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a1", asOFFSET(sead::Matrix34f, a[1])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a2", asOFFSET(sead::Matrix34f, a[2])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a3", asOFFSET(sead::Matrix34f, a[3])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a4", asOFFSET(sead::Matrix34f, a[4])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a5", asOFFSET(sead::Matrix34f, a[5])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a6", asOFFSET(sead::Matrix34f, a[6])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a7", asOFFSET(sead::Matrix34f, a[7])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a8", asOFFSET(sead::Matrix34f, a[8])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a9", asOFFSET(sead::Matrix34f, a[9])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a10", asOFFSET(sead::Matrix34f, a[10])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix34f", "float a11", asOFFSET(sead::Matrix34f, a[11])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Matrix34f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Matrix34fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix34f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float a0, float a1=0, float a2=0, float a3=0, float a4=0, float a5=0, float a6=0, float a7=0, float a8=0, float a9=0, float a10=0, float a11=0)", AngelScript::asFUNCTION(Matrix34fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix34f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Matrix34f &in)", AngelScript::asFUNCTION(Matrix34fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        // transform<->pos+rot conversions
        asErrno = engine->RegisterObjectMethod("Matrix34f", "Vector3f get_pos() property", AngelScript::asFUNCTION(matrix34f_pos_get), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("Matrix34f", "void set_pos(const Vector3f &in) property", AngelScript::asFUNCTION(matrix34f_pos_set), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("Matrix34f", "Matrix33f get_rot() property", AngelScript::asFUNCTION(matrix34f_rot_get), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("Matrix34f", "void set_rot(const Matrix33f &in) property", AngelScript::asFUNCTION(matrix34f_rot_set), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("Matrix34f", "void setPosRot(const Vector3f &in, const Matrix33f &in)", AngelScript::asFUNCTION(matrix34f_posrot_set), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);

        asErrno = engine->RegisterObjectType("Matrix44f", sizeof(sead::Matrix44f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a0", asOFFSET(sead::Matrix44f, a[0])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a1", asOFFSET(sead::Matrix44f, a[1])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a2", asOFFSET(sead::Matrix44f, a[2])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a3", asOFFSET(sead::Matrix44f, a[3])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a4", asOFFSET(sead::Matrix44f, a[4])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a5", asOFFSET(sead::Matrix44f, a[5])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a6", asOFFSET(sead::Matrix44f, a[6])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a7", asOFFSET(sead::Matrix44f, a[7])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a8", asOFFSET(sead::Matrix44f, a[8])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a9", asOFFSET(sead::Matrix44f, a[9])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a10", asOFFSET(sead::Matrix44f, a[10])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a11", asOFFSET(sead::Matrix44f, a[11])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a12", asOFFSET(sead::Matrix44f, a[12])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a13", asOFFSET(sead::Matrix44f, a[13])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a14", asOFFSET(sead::Matrix44f, a[14])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("Matrix44f", "float a15", asOFFSET(sead::Matrix44f, a[15])); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("Matrix44f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(Matrix44fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix44f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float a0, float a1=0, float a2=0, float a3=0, float a4=0, float a5=0, float a6=0, float a7=0, float a8=0, float a9=0, float a10=0, float a11=0, float a12=0, float a13=0, float a14=0, float a15=0)", AngelScript::asFUNCTION(Matrix44fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("Matrix44f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Matrix44f &in)", AngelScript::asFUNCTION(Matrix44fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );

        asErrno = engine->RegisterObjectType("BoundBox2f", sizeof(sead::BoundBox2f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        // TODO operator +- Vector2f, operator */ float, etc
        asErrno = engine->RegisterObjectProperty("BoundBox2f", "Vector2f min", 0); assert(asErrno >= 0); // XXX private access
        asErrno = engine->RegisterObjectProperty("BoundBox2f", "Vector2f max", 8); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox2f", "float min_x",  0); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox2f", "float min_y",  4); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox2f", "float max_x",  8); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox2f", "float max_y", 12); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("BoundBox2f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(BoundBox2fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("BoundBox2f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float min_x, float min_y, float max_x, float max_y)", AngelScript::asFUNCTION(BoundBox2fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("BoundBox2f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Vector2f &in, const Vector2f &in)", AngelScript::asFUNCTION(BoundBox2fAssignConstructorVecVec), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("BoundBox2f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const BoundBox2f &in)", AngelScript::asFUNCTION(BoundBox2fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );

        asErrno = engine->RegisterObjectType("BoundBox3f", sizeof(sead::BoundBox3f), AngelScript::asOBJ_VALUE | AngelScript::asOBJ_POD | AngelScript::asOBJ_APP_CLASS_CAK | AngelScript::asOBJ_APP_CLASS_ALLFLOATS); assert( asErrno >= 0 );
        // TODO operator +- Vector3f, operator */ float, etc
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "Vector3f min", 0); assert(asErrno >= 0); // XXX private access
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "Vector3f max",12); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "float min_x",  0); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "float min_y",  4); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "float min_z",  8); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "float max_x", 12); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "float max_y", 16); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("BoundBox3f", "float max_z", 20); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectBehaviour("BoundBox3f", AngelScript::asBEHAVE_CONSTRUCT, "void f()", AngelScript::asFUNCTION(BoundBox3fConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("BoundBox3f", AngelScript::asBEHAVE_CONSTRUCT, "void f(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z)", AngelScript::asFUNCTION(BoundBox3fAssignConstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("BoundBox3f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const Vector3f &in, const Vector3f &in)", AngelScript::asFUNCTION(BoundBox3fAssignConstructorVecVec), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );
        asErrno = engine->RegisterObjectBehaviour("BoundBox3f", AngelScript::asBEHAVE_CONSTRUCT, "void f(const BoundBox3f &in)", AngelScript::asFUNCTION(BoundBox3fKopyKonstructor), AngelScript::asCALL_CDECL_OBJLAST); assert( asErrno >= 0 );

        // TODO register matrix operations

        // register container statics (AS just uses namespaces for statics)
        engine->SetDefaultNamespace("Vector3f");
        asErrno = engine->RegisterGlobalProperty("const Vector3f ZERO",  &Vector3fStatic::ZERO); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f ONE",   &Vector3fStatic::ONE); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f NEG",   &Vector3fStatic::NEG); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f UP",    &Vector3fStatic::UP); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f U" ,    &Vector3fStatic::UP); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f DOWN",  &Vector3fStatic::DOWN); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f D"   ,  &Vector3fStatic::DOWN); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f EAST",  &Vector3fStatic::EAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f E"   ,  &Vector3fStatic::EAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f WEST",  &Vector3fStatic::WEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f W"   ,  &Vector3fStatic::WEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f NORTH", &Vector3fStatic::NORTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f N"    , &Vector3fStatic::NORTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f SOUTH", &Vector3fStatic::SOUTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f S"    , &Vector3fStatic::SOUTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f NORTHEAST", &Vector3fStatic::NORTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f NE"       , &Vector3fStatic::NORTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f NORTHWEST", &Vector3fStatic::NORTHWEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f NW"       , &Vector3fStatic::NORTHWEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f SOUTHEAST", &Vector3fStatic::SOUTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f SE"       , &Vector3fStatic::SOUTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f SOUTHWEST", &Vector3fStatic::SOUTHWEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Vector3f SW"       , &Vector3fStatic::SOUTHWEST); assert(asErrno >= 0);

        engine->SetDefaultNamespace("Matrix22f");
        asErrno = engine->RegisterGlobalProperty("const Matrix22f IDENTITY", &Matrix22fStatic::IDENTITY); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix22f ZERO",  &Matrix22fStatic::ZERO); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix22f ONE",   &Matrix22fStatic::ONE); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix22f NEG",   &Matrix22fStatic::NEG); assert(asErrno >= 0);

        engine->SetDefaultNamespace("Matrix33f");
        asErrno = engine->RegisterGlobalProperty("const Matrix33f IDENTITY", &Matrix33fStatic::IDENTITY); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f ZERO",  &Matrix33fStatic::ZERO); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f ONE",   &Matrix33fStatic::ONE); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f NEG",   &Matrix33fStatic::NEG); assert(asErrno >= 0);

        asErrno = engine->RegisterGlobalProperty("const Matrix33f EAST",  &Matrix33fStatic::EAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f E"   ,  &Matrix33fStatic::EAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f WEST",  &Matrix33fStatic::WEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f W"   ,  &Matrix33fStatic::WEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f NORTH", &Matrix33fStatic::NORTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f N"    , &Matrix33fStatic::NORTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f SOUTH", &Matrix33fStatic::SOUTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f S"    , &Matrix33fStatic::SOUTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f NORTHEAST", &Matrix33fStatic::NORTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f NE"       , &Matrix33fStatic::NORTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f NORTHWEST", &Matrix33fStatic::NORTHWEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f NW"       , &Matrix33fStatic::NORTHWEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f SOUTHEAST", &Matrix33fStatic::SOUTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f SE"       , &Matrix33fStatic::SOUTHEAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f SOUTHWEST", &Matrix33fStatic::SOUTHWEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f SW"       , &Matrix33fStatic::SOUTHWEST); assert(asErrno >= 0);

        asErrno = engine->RegisterGlobalProperty("const Matrix33f DOWN__UP_IS_EAST", &Matrix33fStatic::DOWN__UP_IS_EAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f DOWN__UP_IS_WEST", &Matrix33fStatic::DOWN__UP_IS_WEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f DOWN__UP_IS_NORTH", &Matrix33fStatic::DOWN__UP_IS_NORTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f DOWN__UP_IS_SOUTH", &Matrix33fStatic::DOWN__UP_IS_SOUTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f UP__UP_IS_EAST", &Matrix33fStatic::UP__UP_IS_EAST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f UP__UP_IS_WEST", &Matrix33fStatic::UP__UP_IS_WEST); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f UP__UP_IS_NORTH", &Matrix33fStatic::UP__UP_IS_NORTH); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix33f UP__UP_IS_SOUTH", &Matrix33fStatic::UP__UP_IS_SOUTH); assert(asErrno >= 0);

        engine->SetDefaultNamespace("Matrix34f");
        asErrno = engine->RegisterGlobalProperty("const Matrix34f ZERO",  &Matrix34fStatic::ZERO); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix34f ONE",   &Matrix34fStatic::ONE); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix34f NEG",   &Matrix34fStatic::NEG); assert(asErrno >= 0);

        engine->SetDefaultNamespace("Matrix44f");
        asErrno = engine->RegisterGlobalProperty("const Matrix44f IDENTITY", &Matrix44fStatic::IDENTITY); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix44f ZERO",  &Matrix44fStatic::ZERO); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix44f ONE",   &Matrix44fStatic::ONE); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalProperty("const Matrix44f NEG",   &Matrix44fStatic::NEG); assert(asErrno >= 0);

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
            asErrno = engine->RegisterGlobalFunction("void wsListen()", AngelScript::asFUNCTION(sys::wsListen), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("ptr_t mainOffset()", AngelScript::asFUNCTION(sys::mainOffset), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("u32 totkVersion()", AngelScript::asFUNCTION(sys::totkVersion), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("u64 tick()", AngelScript::asFUNCTION(sys::tick), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void debugLog(const string &in)", AngelScript::asFUNCTION(sys::debugLog), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("Logger"); /// {
            engine->RegisterGlobalFunction("void logText(const string &in, const string &in = \"/Logger\", bool = false, bool = true )", AngelScript::asFUNCTION(Logger::logText), AngelScript::asCALL_CDECL);
        /// }

        engine->SetDefaultNamespace("HexDump"); /// {
            asErrno = engine->RegisterGlobalFunction("void clearSlot(index_t)", AngelScript::asFUNCTION(lotuskit::HexDump::clearSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void pauseSlot(index_t)", AngelScript::asFUNCTION(lotuskit::HexDump::pauseSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAbsolute(index_t, ptr_t, u32, u32, const string &in = \"HexDump\")", AngelScript::asFUNCTION(lotuskit::HexDump::assignSlotAbsolute), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
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
            // FIXME trash freeze binding
            asErrno = engine->RegisterGlobalFunction("void log()", AngelScript::asFUNCTION(lotuskit::util::camera::log), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("PrimitiveDrawer");
        asErrno = engine->RegisterGlobalFunction("void setModelMtx(index_t, const Matrix34f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::setModelMtx), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void setProjection(index_t, const sead::Projection &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::setProjection), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void setCamera(index_t, const sead::Camera &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::setCamera), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void setDrawCtx(index_t, const sead::DrawContext &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::setDrawCtx), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void drawQuad(index_t, const sead::PrimitiveDrawer::QuadArg &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawQuad), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawQuad(index_t, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawQuad), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawBox(index_t, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawBox), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawWireCube(index_t, const BoundBox3f &in, const Color4f &in)", AngelScript::asFUNCTION(primq_wirecube), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawWireCube(index_t, const Vector3f &in, const Vector3f &in, const Color4f &in)", AngelScript::asFUNCTION(primq_wirecube2), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawLine(index_t, const Vector3f &in, const Vector3f &in, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawLine), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawSphere4x8(index_t, const Vector3f &in, float, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawSphere4x8), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawSphere8x16(index_t, const Vector3f &in, float, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawSphere8x16), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawDisk32(index_t, const Vector3f &in, float, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawDisk32), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawCircle32(index_t, const Vector3f &in, float, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawCircle32), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawCylinder16(index_t, const Vector3f &in, float, float, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawCylinder16), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void drawCylinder32(index_t, const Vector3f &in, float, float, const Color4f &in, const Color4f &in)", AngelScript::asFUNCTION(lotuskit::PrimitiveDrawer::drawCylinder32), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
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

            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_ZERO", &Vector2fStatic::STICK_ZERO); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_UP_MAX", &Vector2fStatic::STICK_UP_MAX); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_UP", &Vector2fStatic::STICK_UP_DIR); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_DOWN_MAX", &Vector2fStatic::STICK_DOWN_MAX); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_DOWN", &Vector2fStatic::STICK_DOWN_DIR); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_LEFT_MAX", &Vector2fStatic::STICK_LEFT_MAX); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_LEFT", &Vector2fStatic::STICK_LEFT_DIR); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_RIGHT_MAX", &Vector2fStatic::STICK_RIGHT_MAX); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("const Vector2f STICK_RIGHT", &Vector2fStatic::STICK_RIGHT_DIR); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("tas"); /// {
            asErrno = engine->RegisterGlobalFunction(
                "void input(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0)",
                AngelScript::asFUNCTION(lotuskit::tas::Playback::setCurrentInput),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                // note these floats still use s32 units, not 0...1! There's no Vector2i binding yet and this seems simpler anyways
                "void input(u32 duration, u64 nextButtons, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_input_vec2f_l),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void input(u32 duration, u64 nextButtons, const Vector2f &in, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_input_vec2f_lr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void input(u32 duration, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_input_vec2f_0l),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void input(u32 duration, const Vector2f &in, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_input_vec2f_0lr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );

            // inputOr allows human input to add buttons
            asErrno = engine->RegisterGlobalFunction(
                "void inputOr(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0)",
                AngelScript::asFUNCTION(lotuskit::tas::Playback::setCurrentInputOr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                // note these floats still use s32 units, not 0...1! There's no Vector2i binding yet and this seems simpler anyways
                "void inputOr(u32 duration, u64 nextButtons, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputor_vec2f_l),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void inputOr(u32 duration, u64 nextButtons, const Vector2f &in, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputor_vec2f_lr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void inputOr(u32 duration, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputor_vec2f_0l),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void inputOr(u32 duration, const Vector2f &in, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputor_vec2f_0lr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );

            // inputXor allows human input to toggle buttons
            asErrno = engine->RegisterGlobalFunction(
                "void inputXor(u32 duration=1, u64 nextButtons=0, s32 nextLStickX=0, s32 nextLStickY=0, s32 nextRStickX=0, s32 nextRStickY=0)",
                AngelScript::asFUNCTION(lotuskit::tas::Playback::setCurrentInputXor),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                // note these floats still use s32 units, not 0...1! There's no Vector2i binding yet and this seems simpler anyways
                "void inputXor(u32 duration, u64 nextButtons, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputxor_vec2f_l),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void inputXor(u32 duration, u64 nextButtons, const Vector2f &in, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputxor_vec2f_lr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void inputXor(u32 duration, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputxor_vec2f_0l),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );
            asErrno = engine->RegisterGlobalFunction(
                "void inputXor(u32 duration, const Vector2f &in, const Vector2f &in)",
                AngelScript::asFUNCTION(tas_inputxor_vec2f_0lr),
                AngelScript::asCALL_CDECL
            ); assert( asErrno >= 0 );

            asErrno = engine->RegisterGlobalFunction(
                "void sleep(u32 duration=1)",
                AngelScript::asFUNCTION(lotuskit::tas::Playback::setSleepInput),
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

            asErrno = engine->RegisterObjectMethod("ActorBase", "Matrix33f get_rot() property", AngelScript::asFUNCTION(actor_rot_get), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void set_rot(const Matrix33f &in) property", AngelScript::asFUNCTION(lotuskit::util::actor::setRot), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setRot(float, float, float, float, float, float, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::setRot9), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setRot(const Matrix33f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::setRot), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setPosRot(float, float, float, float, float, float, float, float, float, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::setPosRot39), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void setPosRot(const Vector3f &in, const Matrix33f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::setPosRot), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);

            asErrno = engine->RegisterObjectMethod("ActorBase", "BoundBox3f getAABB()", AngelScript::asFUNCTION(actor_aabb_get), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "string getName()", AngelScript::asFUNCTION(actor_name_get), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "RigidBody@ getMainRigidBody()", AngelScript::asFUNCTION(lotuskit::util::actor::getMainRigidBody), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);

            // TODO bb access, but should we even bother with exposing bb inheritance? keeping things generic between ainb/asb just isnt gonna be feasible anyways
            //asErrno = engine->RegisterObjectMethod("ActorBase", "AIBlackboard@ getAIBlackboard()", AngelScript::asFUNCTION(lotuskit::util::actor::getAIBlackboard), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            //asErrno = engine->RegisterObjectMethod("ActorBase", "ASBlackboard@ getASBlackboard()", AngelScript::asFUNCTION(lotuskit::util::actor::getASBlackboard), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);
            asErrno = engine->RegisterObjectMethod("ActorBase", "void dumpASBlackboard(u32=0)", AngelScript::asFUNCTION(lotuskit::util::actor::dumpASBlackboard), AngelScript::asCALL_CDECL_OBJFIRST); assert(asErrno >= 0);

            //engine->SetDefaultNamespace("ResidentActors");
            asErrno = engine->RegisterGlobalProperty("ActorBase@ Player", &ResidentActors::Player); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ PlayerCamera", &ResidentActors::PlayerCamera); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalProperty("ActorBase@ EventCamera", &ResidentActors::EventCamera); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("actor"); /// {
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimple), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimpleXYZ), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in, const Vector3f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimplePos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createSimple(string &in, const Vector3f &in, const Matrix33f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createSimplePosRot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatch), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in, float, float, float)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatchXYZ), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in, const Vector3f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatchPos), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void createAndWatch(index_t, string &in, const Vector3f &in, const Matrix33f &in)", AngelScript::asFUNCTION(lotuskit::util::actor::createAndWatchPosRot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        /// }

        engine->SetDefaultNamespace("ActorWatcher"); /// {
            // slot mgmt
            asErrno = engine->RegisterGlobalFunction("void clearSlot(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::clearSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void pauseSlot(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::pauseSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlot(index_t, ActorBase@)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlot(index_t, ptr_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlot), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotPreActor(index_t, ptr_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotPreActor), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            //asErrno = engine->RegisterGlobalFunction("void assignSlotPreActor(index_t, PreActor@)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotPreActor), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAwaitSpawn(index_t, string &in)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotAwaitSpawn), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAwaitRecall(index_t)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotAwaitRecall), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
            asErrno = engine->RegisterGlobalFunction("void assignSlotAwaitBancEntityHash(index_t, u64)", AngelScript::asFUNCTION(lotuskit::ActorWatcher::assignSlotAwaitBancEntityHash), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

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
        asErrno = engine->RegisterObjectMethod("RigidBody", "Vector3f get_vel() property", AngelScript::asMETHOD(phive::RigidBodyBase, getNextLinearVelocity), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void set_vel(const Vector3f &in) property", AngelScript::asMETHOD(phive::RigidBodyBase, requestSetLinearVelocity), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void setVel(const Vector3f &in)", AngelScript::asMETHOD(phive::RigidBodyBase, requestSetLinearVelocity), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void setVel(float, float, float)", AngelScript::asMETHOD(phive::RigidBodyBase, requestSetLinearVelocityXYZ), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void applyImpulse(const Vector3f &in)", AngelScript::asMETHOD(phive::RigidBodyBase, applyLinearImpulse), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "void applyImpulse(float, float, float)", AngelScript::asMETHOD(phive::RigidBodyBase, applyLinearImpulseXYZ), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "BoundBox3f getAABB()", AngelScript::asMETHOD(phive::RigidBodyBase, getAABB), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "BoundBox3f getBoundingBoxWorld()", AngelScript::asMETHOD(phive::RigidBodyBase, getBoundingBoxWorld), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectMethod("RigidBody", "string getName()", AngelScript::asMETHOD(phive::RigidBodyBase, getName), AngelScript::asCALL_THISCALL); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("RigidBody", "Matrix34f lastTransform", asOFFSET(phive::RigidBodyBase, lastTransform)); assert(asErrno >= 0);
        asErrno = engine->RegisterObjectProperty("RigidBody", "Matrix34f prevTransform", asOFFSET(phive::RigidBodyBase, prevTransform)); assert(asErrno >= 0);
    }

    void registerEvent(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;
        engine->SetDefaultNamespace("event");
        asErrno = engine->RegisterGlobalFunction("void requestSimple(string &in, ptr_t=0)",    AngelScript::asFUNCTION(lotuskit::util::event::requestSimple), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void requestSimple(string &in, ActorBase@)", AngelScript::asFUNCTION(lotuskit::util::event::requestSimple), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
    }

    void registerPlayerUtil(AngelScript::asIScriptEngine* engine) {
        //s32 asErrno;
        //engine->SetDefaultNamespace("PlayerUtil");
        //asErrno = engine->RegisterGlobalFunction("void disableGloom(bool)", AngelScript::asFUNCTION(lotuskit::util::player::disableGloom), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void setVel(const Vector3f &in)", AngelScript::asFUNCTION(lotuskit::util::player::setLinearVelocity), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        //asErrno = engine->RegisterGlobalFunction("void setVel(float, float, float)", AngelScript::asFUNCTION(lotuskit::util::player::setLinearVelocityXYZ), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
    }

    void registerPauseUtil(AngelScript::asIScriptEngine* engine) {
        s32 asErrno;
        engine->SetDefaultNamespace("pause");
        asErrno = engine->RegisterGlobalFunction("void doTextWriter(bool)", AngelScript::asFUNCTION(lotuskit::util::pause::doTextWriter_set), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void doTextWriterExtended(bool)", AngelScript::asFUNCTION(lotuskit::util::pause::doTextWriterExtended_set), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

        asErrno = engine->RegisterGlobalFunction("void requestPause(u32)", AngelScript::asFUNCTION(lotuskit::util::pause::requestPause), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void requestPause(const string &in)", AngelScript::asFUNCTION(lotuskit::util::pause::requestPauseStr), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void releasePause(u32)", AngelScript::asFUNCTION(lotuskit::util::pause::releasePause), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void releasePause(const string &in)", AngelScript::asFUNCTION(lotuskit::util::pause::releasePauseStr), AngelScript::asCALL_CDECL); assert(asErrno >= 0);

        asErrno = engine->RegisterGlobalFunction("void doFreezeMask(bool)", AngelScript::asFUNCTION(lotuskit::util::pause::doFreezeMask), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void freezeMask(u32, u32, u32, u32)", AngelScript::asFUNCTION(lotuskit::util::pause::freezeMask4x4), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void freezeRequest(u32 reqHash, bool val=true, bool clearOthers=true)", AngelScript::asFUNCTION(lotuskit::util::pause::freezeRequest), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void freezeRequest(const string &in, bool val=true, bool clearOthers=true)", AngelScript::asFUNCTION(lotuskit::util::pause::freezeRequestStr), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void freezeTarget(u32 targetHash, bool val=true, bool clearOthers=true)", AngelScript::asFUNCTION(lotuskit::util::pause::freezeTarget), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void freezeTarget(const string &in, bool val=true, bool clearOthers=true)", AngelScript::asFUNCTION(lotuskit::util::pause::freezeTargetStr), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
        asErrno = engine->RegisterGlobalFunction("void freezeTargetIndex(u8 targetIndex, bool val=true, bool clearOthers=true)", AngelScript::asFUNCTION(lotuskit::util::pause::freezeTargetIndex), AngelScript::asCALL_CDECL); assert(asErrno >= 0);
    }

    void registerGlobals(AngelScript::asIScriptEngine* engine) {
        registerBaseTypes(engine);
        registerContainers(engine);
        // TODO registerClassFwdDecls(engine); // avoid circular/mutual dep issues
        registerUtil(engine);
        registerTAS(engine);
        registerPhive(engine);
        registerActorSystem(engine);
        registerEvent(engine);
        registerPlayerUtil(engine);
        registerPauseUtil(engine);
    }

} // ns
