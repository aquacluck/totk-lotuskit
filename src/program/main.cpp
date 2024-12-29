#include <string.h>
#include "lib.hpp"
#include <nn/hid.h>
#include <nn/util.h>
#include <heap/seadHeap.h>
#include "syms_merged.hpp"

#include "structs/engineActor.hpp"
#include "Config.hpp"
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
#include "util/romfs.hpp"
using Logger = lotuskit::Logger;


HOOK_DEFINE_INLINE(StealHeap) {
    static const ptrdiff_t s_offset = sym::engine::steal_heap::offset; // hacks
    inline static sead::Heap* stolenHeap = nullptr;
    static void Callback(exl::hook::InlineCtx* ctx) {
#ifdef TOTK_100
        // TODO register annotation/aliasing instead
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[19]);
#else
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[22]);
#endif
        svcOutputDebugString("yoink", 5);
        lotuskit::TextWriter::assignHeap(stolenHeap);
        lotuskit::script::engine::assignHeap(stolenHeap);
        lotuskit::script::engine::createAndConfigureEngine();
    }
};

/*
HOOK_DEFINE_TRAMPOLINE(OnWhistleHook) {
    static const ptrdiff_t s_offset = sym::game::ai::execute::ExecutePlayerWhistle::enterImpl_::offset;
    static void Callback(void* param) {
        testAngelScript();
        Orig(param);
    }
};
*/

HOOK_DEFINE_INLINE(OnRecallUpdateHighlightActorHook) {
    static const ptrdiff_t s_offset = sym::game::ai::execute::ExecutePlayerReverseRecorder::updateImpl_::state0::offset;
    static void Callback(exl::hook::InlineCtx* ctx) {
        auto* highlightedActor = (engine::actor::ActorBase*)(ctx->X[8]);
        lotuskit::ActorWatcher::resolveRecallHighlight(highlightedActor);
    }
};

HOOK_DEFINE_TRAMPOLINE(BaseProcMgr_addDependency) {
    static const ptrdiff_t s_offset = sym::engine::actor::BaseProcMgr::addDependency::offset;

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
    static const ptrdiff_t s_offset = sym::engine::MainGetNpadStates::offset; // hacks

    static void Callback(void* param_1) {
        // TODO option to lock/snap inputs to 90d/45d while ZL is held?
        // TODO see if we can totally stub this. bad sampling number or something if we don't call it, appears to recreate controller stuff every frame `ServiceAm .ctor: Applet 'Controller' created`
        // HLE.OsThread.47 ServiceHid Start: Stubbed. ControllerApplet ArgPriv 20 1072 ShowControllerSupport HoldType:Horizontal StyleSets:ProController, Handheld, JoyconPair
        Orig(param_1);

        constexpr u32 target_idk = 0; // styleset? iterates over 3?
        nn::hid::NpadBaseState* state = (nn::hid::NpadBaseState*)(param_1 + target_idk * 0xe98 + 0x58);
        lotuskit::tas::Playback::applyCurrentInput(state);
        lotuskit::tas::Record::applyCurrentInput(state);
    }
};

HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static const auto s_offset = sym::game::wm::WorldManagerModule::baseProcExe::offset;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        //Logger::logText("ffffeeeeddddcccc", "/HexDump/0", true); // blocking ws
        //Logger::logJson(json::object({{"kee", "vee"}, {"k2", 420}}));
        // TODO source+display high level configs (socket, svclog, etc)?

        if (lotuskit::ActorWatcher::slots[0].actor == nullptr) {
            lotuskit::TextWriter::printf(0, "[totk-lotuskit:%d] awaiting Player, main_offset=%p\n", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        }

        // drawlist 0 near top-left (default)
        // drawlist 1 near top-right
        lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
            textPos->x = 1280.0 - 150.0;
            textPos->y = 2.0;
        });

        lotuskit::tas::Playback::calc(); // may re-enter script when currently scheduled input is complete
        lotuskit::tas::Record::calc();
        lotuskit::tas::InputDisplay::draw();

        lotuskit::server::WebSocket::calc(); // noblock recv, but blocking processing if enabled
        lotuskit::ActorWatcher::calc();
        lotuskit::HexDump::calc();

        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};

HOOK_DEFINE_INLINE(nnMainHook) {
    static const ptrdiff_t s_offset = sym::engine::nnMain_post_setup::offset; // hacks
    static void Callback(exl::hook::InlineCtx* ctx) {
        // Effective entry point after sdk init
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] nnMainHook main_offset=%p", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        svcOutputDebugString(buf, strlen(buf));

        lotuskit::Config::loadJson();
        auto& config = lotuskit::Config::jsonConfig;

        const json::json_pointer GLOBAL_DISABLE("/global/disable");
        if (config.contains(GLOBAL_DISABLE) && config[GLOBAL_DISABLE]) {
            nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] all features disabled", TOTK_VERSION);
            svcOutputDebugString(buf, strlen(buf));
            return;
        }

        lotuskit::server::WebSocket::calc(); // blocking if listenOnBootup
        WorldManagerModuleBaseProcHook::Install(); // "main loop"
        StealHeap::Install(); // called once, a bit later during bootup
        //OnWhistleHook::Install();
        OnRecallUpdateHighlightActorHook::Install();
        BaseProcMgr_addDependency::Install();
        MainGetNpadStates::Install();

        // hooks for textwriter+primitivedrawer overlay
        bool do_textwriter = (
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font.ntx") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_jis1.ntx") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_jis1_mipmap.xtx") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_jis1_tbl.bin") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_shader.bin") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_shader_jis1.bin") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/nvn_font/nvn_font_shader_jis1_mipmap.bin") &&
            lotuskit::util::romfs::fileExists("content:/Lib/sead/primitive_renderer/primitive_drawer_nvn_shader.bin")
        );
        if (do_textwriter) {
            lotuskit::TextWriterHooks::BootupInitDebugDrawersHook::Install();
            lotuskit::TextWriterHooks::DebugDrawLayerMaskHook::Install();
            lotuskit::TextWriterHooks::DebugDrawHook::Install();
            lotuskit::TextWriterHooks::AglRendererDrawHook::Install();
            lotuskit::PrimitiveImpl::setupStatic();
        }

        //InputHelper::initKBM();
        //InputHelper::setPort(0); // default controller port
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    // if you launch the wrong version this is all you'll see
    char buf[] = "[totk-lotuskit:___] exl_main";
    buf[15] = '0' + TOTK_VERSION/100;
    buf[16] = '0' + TOTK_VERSION%100/10;
    buf[17] = '0' + TOTK_VERSION%10;
    svcOutputDebugString(buf, strlen(buf));

    exl::hook::Initialize();
    nnMainHook::Install();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
