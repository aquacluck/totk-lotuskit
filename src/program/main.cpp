#include <string.h>
#include "exlaunch.hpp"
#include <nn/hid.h>
#include <nn/util.h>
#include <heap/seadHeap.h>

#include "structs/engineActor.hpp"
#include "Logger.hpp"
#include "ActorWatcher.hpp"
#include "HexDump.hpp"
#include "TextWriter.hpp"
#include "PrimitiveDrawer.hpp"
#include "server/WebSocket.hpp"
#include "script/engine.hpp"
#include "script/globals.hpp"
#include "tas/InputDisplay.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"
#include "util/camera.hpp"
#include "util/romfs.hpp"
using Logger = lotuskit::Logger;


HOOK_DEFINE_INLINE(StealHeap) {
    static constexpr auto s_name = "engine::steal_heap"; // hacks
    inline static sead::Heap* stolenHeap = nullptr;
    static void Callback(exl::hook::InlineCtx* ctx) {
#ifdef TOTK_100
        // TODO register annotation/aliasing instead
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[19]);
#else
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[22]);
#endif
        svcOutputDebugString("yoink", 5);
        lotuskit::server::WebSocket::assignHeap(stolenHeap);
        lotuskit::TextWriter::assignHeap(stolenHeap);
        lotuskit::PrimitiveImpl::assignHeap(stolenHeap);
        lotuskit::script::engine::assignHeap(stolenHeap);
        lotuskit::script::engine::createAndConfigureEngine();
    }
};

/*
HOOK_DEFINE_TRAMPOLINE(OnWhistleHook) {
    static constexpr auto s_name = "game::ai::execute::ExecutePlayerWhistle::enterImpl_";
    static void Callback(void* param) {
        lotuskit::server::WebSocket::init(); // blocking
        Orig(param);
    }
};
*/

HOOK_DEFINE_INLINE(OnRecallUpdateHighlightActorHook) {
    static constexpr auto s_name = "game::ai::execute::ExecutePlayerReverseRecorder::updateImpl_::state0";
    static void Callback(exl::hook::InlineCtx* ctx) {
        auto* highlightedActor = (engine::actor::ActorBase*)(ctx->X[8]);
        lotuskit::ActorWatcher::resolveRecallHighlight(highlightedActor);
    }
};

HOOK_DEFINE_TRAMPOLINE(OnRequestCreateActorAsyncHook) {
    // async is most actors, exceptions: actors created as a dependency?
    static constexpr auto s_name = "engine::actor::ActorMgr::requestCreateActorAsync";

    static void* Callback
        (engine::actor::ActorMgr* actorMgr, char** actorName, sead::Vector3f* pos, void* param_4, u32 param_5, engine::actor::PreActor* preActor,
        void* actorObserver, void* param_8, u8 param_9, u32* param_10, engine::actor::PreActor** destPreActor) {

        if (actorName == nullptr || *actorName == nullptr) {
            return Orig(actorMgr, actorName, pos, param_4, param_5, preActor, actorObserver, param_8, param_9, param_10, destPreActor);
        }

        s64 slot_i = lotuskit::ActorWatcher::querySpawnSelectorSlot(*actorName);
        if (slot_i < 0) {
            // default case: no ActorWatcher name match
            if (preActor == nullptr) {
                // XXX banc entities/actors/??? always have preactor passed?
                // early return, handling non-banc dynamic spawns ???
                return Orig(actorMgr, actorName, pos, param_4, param_5, preActor, actorObserver, param_8, param_9, param_10, destPreActor);
            }

            slot_i = -1;
            if (preActor->mInstanceId) { slot_i = lotuskit::ActorWatcher::querySpawnBancHashSlot(preActor->mInstanceId); }
            if (slot_i >= 0) { lotuskit::ActorWatcher::assignSlotPreActor(slot_i, preActor); }
            return Orig(actorMgr, actorName, pos, param_4, param_5, preActor, actorObserver, param_8, param_9, param_10, destPreActor);
        }

        // ActorWatcher name match: something with destPreActor seems crashy if we do it for everything? seems safe scoped down like this.

        // inject our own dest pointer if needed
        engine::actor::PreActor* pa = nullptr;
        if (destPreActor == nullptr) {
            destPreActor = &pa;
        }

        void* ret = nullptr;
        ret = Orig(actorMgr, actorName, pos, param_4, param_5, preActor, actorObserver, param_8, param_9, param_10, destPreActor);
        pa = *destPreActor;

        if (pa != nullptr && actorName && *actorName) {
            /*// stuff tends to crash?
            Logger::logJson(json::object({
                {"actorName", *actorName},
                {"pos", json::array({ pos->x, pos->y, pos->z })}, // rarely has placement
                {"preactorPtr", (u64)pa},
                {"preactorPos", json::array({ pa->mPosition.x, pa->mPosition.y, pa->mPosition.z })} // usually has placement
            }), "/hook/sym/engine/actor/ActorMgr/requestCreateActorAsync");
            */

            // resolve "next spawn" actor selection. if many match in a burst, whichever writes last will win?
            lotuskit::ActorWatcher::assignSlotPreActor(slot_i, pa);
        }

        return ret;
    }
};

HOOK_DEFINE_TRAMPOLINE(BaseProcMgr_addDependency) {
    static constexpr auto s_name = "engine::actor::BaseProcMgr::addDependency";

    static u32 Callback(engine::actor::BaseProcMgr &baseProcMgr, engine::actor::ActorBase &parent, engine::actor::ActorBase &child) {
        u32 result = Orig(baseProcMgr, parent, child);

        // register ResidentActors for use everywhere
        if (!strcmp(parent.mName.cstr(), "Player") && lotuskit::script::globals::ResidentActors::Player != &parent) {
            lotuskit::script::globals::ResidentActors::Player = &parent;
            Logger::logJson(json::object({ {"Player", (u64)(&parent)} }), "/hook/sym/BaseProcMgr_addDependency");
            lotuskit::ActorWatcher::assignSlot(0, &parent);
        }
        if (!strcmp(parent.mName.cstr(), "PlayerCamera") && lotuskit::script::globals::ResidentActors::PlayerCamera != &parent) {
            lotuskit::script::globals::ResidentActors::PlayerCamera = &parent;
            Logger::logJson(json::object({ {"PlayerCamera", (u64)(&parent)} }), "/hook/sym/BaseProcMgr_addDependency");
        }
        if (!strcmp(parent.mName.cstr(), "EventCamera") && lotuskit::script::globals::ResidentActors::EventCamera != &parent) {
            lotuskit::script::globals::ResidentActors::EventCamera = &parent;
            Logger::logJson(json::object({ {"EventCamera", (u64)(&parent)} }), "/hook/sym/BaseProcMgr_addDependency");
        }

        // TODO logging settings
        if (false) {
            Logger::logJson(json::object({
                {"parent", json::object({
                    {"name", parent.mName.cstr()},
                    {"ringSize", parent.mDependencyRing.mSize}
                })},
                {"child", json::object({
                    {"name", child.mName.cstr()},
                    {"ringSize", child.mDependencyRing.mSize}
                })}
            }), "/hook/sym/BaseProcMgr_addDependency");
        }

        return result;
    }
};

HOOK_DEFINE_TRAMPOLINE(MainGetNpadStates) {
    static constexpr auto s_name = "engine::MainGetNpadStates"; // hacks

    static void Callback(void* param_1) {
        // TODO option to lock/snap inputs to 90d/45d while ZL is held?
        // TODO see if we can totally stub this. bad sampling number or something if we don't call it, appears to recreate controller stuff every frame `ServiceAm .ctor: Applet 'Controller' created`
        // HLE.OsThread.47 ServiceHid Start: Stubbed. ControllerApplet ArgPriv 20 1072 ShowControllerSupport HoldType:Horizontal StyleSets:ProController, Handheld, JoyconPair
        Orig(param_1);

        constexpr u32 target_idk = 0; // styleset? iterates over 3?
        nn::hid::NpadBaseState* state = (nn::hid::NpadBaseState*)(param_1 + target_idk * 0xe98 + 0x58);
        lotuskit::tas::Record::applyCurrentInput(state);
        lotuskit::tas::Playback::applyCurrentInput(state);
    }
};

HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static constexpr auto s_name = "game::wm::WorldManagerModule::baseProcExe";

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        //Logger::logText("ffffeeeeddddcccc", "/HexDump/0", true); // blocking ws
        //Logger::logJson(json::object({{"kee", "vee"}, {"k2", 420}}));

        if (lotuskit::ActorWatcher::slots[0].actor == nullptr) {
            lotuskit::TextWriter::printf(0, "[totk-lotuskit:%d] awaiting Player, main_offset=%p\nStart WS server anytime: L R ZL ZR + - \n", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        }

        // drawlist 0 near top-left (default)
        // drawlist 1 near top-right
        lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
            textPos->x = 1280.0 - 155.0;
            textPos->y = 2.0;
        });

        lotuskit::tas::Playback::calc(); // may re-enter script when currently scheduled input is complete
        lotuskit::tas::Record::calc();
        lotuskit::tas::InputDisplay::draw();

        lotuskit::server::WebSocket::calc(); // noblock recv, but blocking processing if enabled

        Orig(self, param_2, param_3, param_4, wmmodule, param_6);

        lotuskit::ActorWatcher::calc();
        lotuskit::HexDump::calc();
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();

    char buf[200];
    nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] exl_main main_offset=%p", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
    svcOutputDebugString(buf, strlen(buf));

    WorldManagerModuleBaseProcHook::Install(); // "main loop"
    StealHeap::Install(); // called once, a bit later during bootup
    //OnWhistleHook::Install();
    OnRecallUpdateHighlightActorHook::Install();
    OnRequestCreateActorAsyncHook::Install();
    BaseProcMgr_addDependency::Install();
    MainGetNpadStates::Install();

    //TODO check the branch we're overwriting -- ensure our "off" does the same thing
    exl::patch::CodePatcher(EXL_SYM_OFFSET("game::component::GameCameraParam::HACK_cameraCalc")).BranchLinkInst((void*)lotuskit::util::camera::disgustingCameraHook);

    // hooks for textwriter+primitivedrawer overlay
    bool do_debugdraw = true; // XXX assert -- can't access fs here without sdk init?
    /*
    bool do_debugdraw = (
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font.ntx") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_jis1.ntx") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_jis1_mipmap.xtx") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_jis1_tbl.bin") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_shader.bin") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_shader_jis1.bin") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_shader_jis1_mipmap.bin") &&
        lotuskit::util::romfs::fileExists("content:/Lib/sead/primitive_renderer/primitive_drawer_nvn_shader.bin")
    );
    */
    if (do_debugdraw) {
        lotuskit::DebugDrawHooks::BootupInitDebugDrawersHook::Install();
        lotuskit::DebugDrawHooks::DebugDrawLayerMaskHook::Install();
        lotuskit::DebugDrawHooks::DebugDrawHook::Install();
        lotuskit::PrimitiveImpl::setupStatic();
    }

    //InputHelper::initKBM();
    //InputHelper::setPort(0); // default controller port
}

// Note: this is only applicable in the context of applets/sysmodules
extern "C" NORETURN void exl_exception_entry() { EXL_ABORT("unreachable abort"); }

