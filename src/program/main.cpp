#include <string.h>
#include "exlaunch.hpp"
#include <nn/fs.h>
#include <nn/hid.h>
#include <nn/util.h>
#include <heap/seadArena.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

#include "structs/engineActor.hpp"
#include "structs/nnSixAxis.hpp"
#include "Logger.hpp"
#include "ActorWatcher.hpp"
#include "HexDump.hpp"
#include "TextWriter.hpp"
#include "PrimitiveDrawer.hpp"
#include "server/WebSocket.hpp"
#include "script/engine.hpp"
#include "script/globals.hpp"
#include "script/hotkey.hpp"
#include "tas/InputDisplay.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"
#include "util/camera.hpp"
#include "util/player.hpp"
#include "util/pause.hpp"
#include "util/world.hpp"
using Logger = lotuskit::Logger;


HOOK_DEFINE_INLINE(StealHeapHook) {
    static constexpr auto s_name = "engine::steal_heap"; // hacks
    inline static sead::Heap* stolenHeap = nullptr;
    static void Callback(exl::hook::InlineCtx* ctx) {
        constexpr auto xi = TOTK_VERSION == 100 ? 19 : 22;
        stolenHeap = reinterpret_cast<sead::Heap*>(ctx->X[xi]);
        lotuskit::TextWriter::assignHeap(StealHeapHook::stolenHeap);
        lotuskit::PrimitiveImpl::assignHeap(StealHeapHook::stolenHeap);

        char buf[32];
        nn::util::SNPrintf(buf, sizeof(buf), "yoink(%p)", stolenHeap);
        svcOutputDebugString(buf, strlen(buf));
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
            lotuskit::ActorWatcher::assignSlot(0, &parent);
            Logger::logJson(json::object({ {"Player", (u64)(&parent)} }), "/hook/sym/BaseProcMgr_addDependency");
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

HOOK_DEFINE_TRAMPOLINE(NinJoyNpadDevice_calcHook) {
    static constexpr auto s_name = "sead::NinJoyNpadDevice::calc"; // hacks

    static void Callback(void* param_1) {
        // TODO option to lock/snap inputs to 90d/45d while ZL is held?
        // TODO see if we can totally stub this. bad sampling number or something if we don't call it, appears to recreate controller stuff every frame `ServiceAm .ctor: Applet 'Controller' created`
        // HLE.OsThread.47 ServiceHid Start: Stubbed. ControllerApplet ArgPriv 20 1072 ShowControllerSupport HoldType:Horizontal StyleSets:ProController, Handheld, JoyconPair
        Orig(param_1);

        constexpr u32 target_idk = 0; // styleset? iterates over 3?
        nn::hid::NpadBaseState* state = (nn::hid::NpadBaseState*)(param_1 + target_idk * 0xe98 + 0x58);
        nn::hid::SixAxisSensorState* state_gyro = (nn::hid::SixAxisSensorState*)(param_1 + target_idk * 0xe98 + 0x2e8);
        //lotuskit::TextWriter::printf(0, "[NinJoyNpadDevice::calc] gyro ptr %p\n", state_gyro);
        lotuskit::tas::Record::applyCurrentInput(state); // keep track of latest human inputs regardless of record state
        lotuskit::tas::Record::applyCurrentGyro(state_gyro);
        bool isInputMutate = lotuskit::tas::Playback::applyCurrentInput(state); // mutate input using current script settings (eg apply user passthrough)
        bool isGyroMutate = lotuskit::tas::Playback::applyCurrentGyro(state_gyro);
        // inject mutated states back into record, so we can re-log final inputs during playback (eg simultaneously playback AS while flattening into nxtas recording)
        // FIXME i think recording injected inputs lags+extends inputs a frame due to AS calc interaction with Framework draw/calc frame boundary?
        if (isInputMutate) { lotuskit::tas::Record::applyCurrentInput(state); }
        if (isGyroMutate)  { lotuskit::tas::Record::applyCurrentGyro(state_gyro); }
    }
};

#define ENABLE_LOG_HEAP_MIN_FREE (false && TOTK_VERSION == 100)
#if ENABLE_LOG_HEAP_MIN_FREE
static u32 minFree[30] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
const char* watchHeaps[30] = {"ActorInstanceHeap", "SaveLoad", "EventHeap", "Sound", "GameUIWorkHeap", "NnVfxDataHeap", "VfxDynamicHeap", "ModuleSystem", "Create Terrain Scene Heap", "Cave Module", "agl::WorkHeap", "UIHeap", "EffectModel", "BancModule", "CreateModelScene", "Chm Module Heap L", "NavMeshMgr", "ReplaceModelHeap", "StaticCompoundBodyMgr", "Sound", "aglTextureDataMgr::Unit", "Terrain Resource Heap", "GameTerrain", "DummyGeneralFormatParamMgrHeap", "Terrain Temp Heap", "Chm Module Heap S", "PlayReporter", "Challenge", "zsdic", "NinVirtualAddressHeap"};
void heap_postalloc(sead::Heap* heap) {
    const char* name = heap->getName().cstr();
    for(int i=0; i < 30; i++) {
        if (!strcmp(name, watchHeaps[i])) {
            // XXX multiple of same name isn't correctly handled anywhere (ModuleSystem, Sound, Challenge, GameTerrain)
            auto free = heap->getFreeSize();
            if (free < minFree[i]) {
                minFree[i] = free;
            }
            break;
        }
    }
}
void heap_print_min_free_impl(sead::Heap* dumproot) {
    sead::Heap* h = dumproot; // current/visiting entry
    sead::Heap* htmp = nullptr;
    u32 depth = 0;

    HEAPINFO_VISIT:
    for(int i=0; i < 30; i++) {
        if (strcmp(h->getName().cstr(), watchHeaps[i]) != 0) {
            continue;
        }
        // log the entry
        if (minFree[i] == 0xffffffff) {
            lotuskit::TextWriter::printf(0, "no_alloc %08x %08x %s(%p)\n", h->getFreeSize(), h->getSize(), watchHeaps[i], h);
        } else {
            lotuskit::TextWriter::printf(0, "%08x %08x %08x %s(%p)\n", minFree[i], h->getFreeSize(), h->getSize(), watchHeaps[i], h);
        }
        break;
    }

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
        // assert h == dumproot
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
void heap_print_min_free() {
    lotuskit::TextWriter::printf(0, "min_free     free     size name(addr):\n");
    heap_print_min_free_impl(*EXL_SYM_RESOLVE<sead::Heap**>("_ZN4sead7HeapMgr22sNinVirtualAddressHeapE"));
    heap_print_min_free_impl(sead::HeapMgr::sRootHeaps[0]);
}
HOOK_DEFINE_TRAMPOLINE(SeadHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadExpHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadExpHeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadFrameHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadSeparateHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadSeparateHeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadSpareExpHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadSpareExpHeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadSpareFrameHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadUnknown0HeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadUnknown1HeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadUnknown1HeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadUnknown2HeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadNinStandardAllocatorHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadNinStandardAllocatorHeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadUnitHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadUnitHeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadVirtualAddressHeapTryAllocHook) {
    static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
        auto ret = Orig(self, size, alignment);
        heap_postalloc(self);
        return ret;
    }
};
HOOK_DEFINE_TRAMPOLINE(SeadVirtualAddressHeapFreeHook) {
    static void Callback(sead::Heap* self, void* ptr) {
        heap_postalloc(self);
        Orig(self, ptr);
    }
};
#endif

/*
HOOK_DEFINE_TRAMPOLINE(FrameworkProcCalcHook) {
    static constexpr auto s_name = "engine::framework::Framework::procCalc_";

    static void Callback(void* self) {
        Orig(self); // perform observations after calc for fresh results
    }
};
*/

HOOK_DEFINE_TRAMPOLINE(FrameworkProcDrawHook) {
    static constexpr auto s_name = "engine::framework::Framework::procDraw_";

    static void Callback(void* self) {
        // drawlist 1 near top-right for tas
        lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
            textPos->x = 1280.0 - 155.0;
            textPos->y = 2.0;
        });
        lotuskit::server::WebSocket::calc(); // noblock recv, but blocking processing if enabled
        lotuskit::script::hotkey::calc(); // may schedule tas stackframes for Playback (incurs file read)
        lotuskit::tas::Playback::calc(); // may re-enter script when currently scheduled input is complete
        lotuskit::tas::Record::calc();
        lotuskit::tas::InputDisplay::draw();
        lotuskit::util::pause::drawPauses();
        lotuskit::ActorWatcher::calc(); // may resolve actor selection via preactor
        lotuskit::HexDump::calc();

        #if ENABLE_LOG_HEAP_MIN_FREE
        heap_print_min_free();
        #endif

        Orig(self); // perform tas+script+scheduling before draw for synced display
    }
};

/*
HOOK_DEFINE_TRAMPOLINE(WorldManagerModuleBaseProcHook) {
    static constexpr auto s_name = "game::wm::WorldManagerModule::baseProcExe";
    inline static u64 ctr = 0;

    static void Callback(double self, double param_2, double param_3, double param_4, void *wmmodule, void *param_6) {
        lotuskit::TextWriter::printf(0, "[wm::baseProcExe] ctr %llu \n", ctr++);
        Orig(self, param_2, param_3, param_4, wmmodule, param_6);
    }
};
*/

HOOK_DEFINE_TRAMPOLINE(InitLotuskitOnTitleScreenHook) {
    static constexpr auto s_name = "game::ai::query::QueryIsTitleMuralEnable::calc";
    static inline s8 defer_init_callcount = 1; // XXX 0 never fires oops

    static void Callback(void* self) {
        Orig(self);

        // ignore n calls, proc once, ignore all further calls
        if (defer_init_callcount < 0) { return; }
        if (--defer_init_callcount != 0) { return; }

        // main mod init
        nn::fs::MountSdCard("sdcard");
        nn::hid::InitializeKeyboard(); // for hotkey
        lotuskit::server::WebSocket::assignHeap(StealHeapHook::stolenHeap);
        lotuskit::TextWriter::createFrameHeap();
        lotuskit::PrimitiveImpl::createFrameHeap();
        lotuskit::script::engine::assignHeap(StealHeapHook::stolenHeap);
        lotuskit::script::engine::createAndConfigureEngine();
        lotuskit::script::engine::doAutorun();

        //WorldManagerModuleBaseProcHook::Install(); // "main loop"
        //FrameworkProcCalcHook::Install();
        FrameworkProcDrawHook::Install();

        //OnWhistleHook::Install();
        OnRecallUpdateHighlightActorHook::Install();
        OnRequestCreateActorAsyncHook::Install();
        NinJoyNpadDevice_calcHook::Install();
        lotuskit::util::player::InstallHooks();
        lotuskit::util::world::InstallHooks();

        //TODO check the branch we're overwriting -- ensure our "off" does the same thing
        exl::patch::CodePatcher(EXL_SYM_OFFSET("game::component::GameCameraParam::HACK_cameraCalc")).BranchLinkInst((void*)lotuskit::util::camera::disgustingCameraHook);

        lotuskit::DebugDrawHooks::DebugDrawLayerMaskHook::Install();
        lotuskit::DebugDrawHooks::DebugDrawHook::Install();
        lotuskit::PrimitiveImpl::setupStatic();
    }
};

/*
struct TestGetHeapSizeHookStruct { size_t size; size_t has_value; size_t more_bigger; };
HOOK_DEFINE_TRAMPOLINE(TestGetHeapSizeHook) {
    static constexpr auto s_name = "engine::util::HeapSizeDB::getHeapSize";
    static TestGetHeapSizeHookStruct Callback(void* idk, sead::SafeString* name) {
        auto ret = Orig(idk, name);
        char buf[200];
        if (ret.has_value) {
            //if (!strcmp(name->cstr(), "AS")) { ret.size = 256; } // XXX try to break it
            nn::util::SNPrintf(buf, sizeof(buf), "getHeapSize(%s) size=%p", name->cstr(), ret.size);
        } else {
            nn::util::SNPrintf(buf, sizeof(buf), "getHeapSize(%s) no_size", name->cstr());
        }
        svcOutputDebugString(buf, strlen(buf));
        return ret;
    }
};
*/

HOOK_DEFINE_TRAMPOLINE(AresSystemInitializeHook) {
    static constexpr auto s_name = "ares::System::initialize";
    static void Callback(void* aresSystem, size_t* aresSystemInitializeArg) {
        aresSystemInitializeArg[21] = 0x80000; // half size zsdic heap
        Orig(aresSystem, aresSystemInitializeArg);
    }
};

HOOK_DEFINE_TRAMPOLINE(DisablePrepoHook) {
    static constexpr auto s_name = "engine::erepo::PlayReportModule::prepare_";
    static void Callback(void* self, void* ptr) { }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();

    char buf[200];
    nn::util::SNPrintf(buf, sizeof(buf), "[totk-lotuskit:%d] exl_main main_offset=%p", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
    svcOutputDebugString(buf, strlen(buf));

    #if ENABLE_LOG_HEAP_MIN_FREE
    // XXX 1.0 symbols only
    SeadHeapTryAllocHook::InstallAtOffset(0x010dc358);
    //SeadHeapFreeHook
    SeadExpHeapTryAllocHook::InstallAtOffset(0x00b21d50);
    SeadExpHeapFreeHook::InstallAtOffset(0x029b72fc);
    SeadFrameHeapTryAllocHook::InstallAtOffset(0x00b21d50);
    //SeadFrameHeapFreeHook
    SeadSeparateHeapTryAllocHook::InstallAtOffset(0x029bd888);
    SeadSeparateHeapFreeHook::InstallAtOffset(0x029bdad4);
    SeadSpareExpHeapTryAllocHook::InstallAtOffset(0x00b21ab0);
    SeadSpareExpHeapFreeHook::InstallAtOffset(0x029b83a8);
    SeadSpareFrameHeapTryAllocHook::InstallAtOffset(0x029b8940);
    //SeadSpareFrameHeapFreeHook
    SeadUnknown0HeapTryAllocHook::InstallAtOffset(0x02a5f560);
    //SeadUnknown0HeapFreeHook::InstallAtOffset(0x02a5f5ac); // empty impl (frameheap?)
    SeadUnknown1HeapTryAllocHook::InstallAtOffset(0x02a609a0);
    SeadUnknown1HeapFreeHook::InstallAtOffset(0x02a60a00);
    SeadUnknown2HeapTryAllocHook::InstallAtOffset(0x02a61998);
    //SeadUnknown2HeapFreeHook::InstallAtOffset(0x02a619e4); // empty impl (frameheap?)
    SeadNinStandardAllocatorHeapTryAllocHook::InstallAtOffset(0x02a7fd38);
    SeadNinStandardAllocatorHeapFreeHook::InstallAtOffset(0x02a7fde4);
    SeadUnitHeapTryAllocHook::InstallAtOffset(0x029b90b8);
    SeadUnitHeapFreeHook::InstallAtOffset(0x029b91d0);
    SeadVirtualAddressHeapTryAllocHook::InstallAtOffset(0x008421d4);
    SeadVirtualAddressHeapFreeHook::InstallAtOffset(0x02150920);
    #endif

    // memory is tight until engine+game init is settled, so we defer most initialization until then
    DisablePrepoHook::Install(); // saves ~1MB
    AresSystemInitializeHook::Install(); // saves 0.5MB
    StealHeapHook::Install(); // called once mid bootup
    InitLotuskitOnTitleScreenHook::Install(); // first called on title screen, main mod init
    BaseProcMgr_addDependency::Install(); // XXX used to locate Player globally, could be deferred otherwise
    // XXX assert textwriter+primitivedrawer deps available
    lotuskit::DebugDrawHooks::BootupInitDebugDrawersHook::Install();
    lotuskit::util::pause::InstallHooks(); // XXX hooks PauseMgr init, could be deferred otherwise
}

// Note: this is only applicable in the context of applets/sysmodules
extern "C" NORETURN void exl_exception_entry() { EXL_ABORT("unreachable abort"); }

