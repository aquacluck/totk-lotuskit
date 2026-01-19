#include "exlaunch.hpp"
#include "structs/VFRMgr.hpp"
#include "util/fps.hpp"
#include "util/patch.hpp"
#include "TextWriter.hpp"

namespace lotuskit::util::fps {
    constexpr u16 MAX_FPS = 30; // not relevant to fps calc, just alloc enough samples to cover 1s
    u64 statDeltaTicks[MAX_FPS];
    u8 statDeltaFrames60[MAX_FPS];
    u16 stat_i = 0;

    void CalcFrame(u64* outDeltaTicks, u8* outDeltaFrames60, float* outAvgDeltaTicks, float* outAvgDeltaFrames60) {
        static u64 prevTick = 0;
        u64 nowTick = svcGetSystemTick();
        *outDeltaTicks = nowTick - prevTick;
        prevTick = nowTick;
        stat_i = (stat_i+1) % MAX_FPS;
        statDeltaTicks[stat_i] = *outDeltaTicks;

        // int 2 or 3 = float 1.0 (@30fps) or 1.5 (@20fps)
        // assert(mDeltaFrame == 1.0 || mDeltaFrame == 1.5); // precisely
        VFRMgr* vfrMgr = *EXL_SYM_RESOLVE<VFRMgr**>("engine::module::VFRMgr::sInstance");
        *outDeltaFrames60 = statDeltaFrames60[stat_i] = (u32)(vfrMgr->mDeltaFrame * 2);

        u64 sumDeltaTicks = 0;
        u8 sumDeltaFrames60 = 0;
        for (int j=0; j < MAX_FPS; j++) {
            sumDeltaTicks += statDeltaTicks[j];
            sumDeltaFrames60 += statDeltaFrames60[j];
        }
        *outAvgDeltaTicks = sumDeltaTicks / MAX_FPS;
        *outAvgDeltaFrames60 = sumDeltaFrames60 / MAX_FPS;
    }

    void PrintStats() {
        if (!doTextWriter) { return; } // don't even collect stats

        u64 deltaTicks;
        u8 deltaFrames60;
        float avgDeltaTicks;
        float avgDeltaFrames60;
        CalcFrame(&deltaTicks, &deltaFrames60, &avgDeltaTicks, &avgDeltaFrames60);

        // ticks are 19200000Hz https://switchbrew.org/wiki/SVC#svcGetSystemTick
        float instTickFps = 19200000.0 / deltaTicks;
        float  avgTickFps = 19200000.0 / avgDeltaTicks;
        float  instVFRFps = 60.0 / deltaFrames60;
        float   avgVFRFps = 60.0 / avgDeltaFrames60;

        lotuskit::TextWriter::printf(1, "rta(fps, 1s): %.1f, %.1f\nvfr(fps, 1s): %.1f, %.1f\n", instTickFps, avgTickFps, instVFRFps, avgVFRFps);
    }

    void wsAnnounceConfig() {
        Logger::logJson(json::object({
            {"doTextWriter", doTextWriter},
            {"fixedFPS", fixedFPS},
        }), "/fps/log", false, false); // noblock, no debug log
    }

    void fixedFPS_set(u8 v) {
        if (v == 0 && fixedFPS != 0) {
            // vanilla
            // FIXME update AS input scaling: ModuleStackFrameScheduleState.inputFPSMode = InputDurationScalingStrategy::FPS30_2X;
            lotuskit::util::patch::PatchRevert("lotuskit-fixed-30fps"); // assert it can revert 20fps too
            fixedFPS = v;

        } else if (v == 20 && fixedFPS != 20) {
            // FIXME update AS input scaling: ModuleStackFrameScheduleState.inputFPSMode = InputDurationScalingStrategy::FPS20_3X;
            lotuskit::util::patch::PatchInstall("lotuskit-fixed-20fps");
            fixedFPS = v;

        } else if (v == 30 && fixedFPS != 30) {
            // FIXME update AS input scaling: ModuleStackFrameScheduleState.inputFPSMode = InputDurationScalingStrategy::FPS30_2X;
            lotuskit::util::patch::PatchInstall("lotuskit-fixed-30fps");
            fixedFPS = v;

        } // else assert v in [0, 20, 30]
        wsAnnounceConfig();
    }

    /*
    HOOK_DEFINE_TRAMPOLINE(EngineFrameworkGpuHook) {
        static constexpr auto s_name = "engine::framework::Framework::setGpuTimeStamp_";
        static void Callback(void* self) {
            Orig(self);

            u64* gpuTimeStamp = (u64*)(self + 0x118); // ~500k is normal on console, ~200k on ryu
            lotuskit::TextWriter::printf(0, "gpuTimeStamp=%d\n", *gpuTimeStamp);

            // *gpuTimeStamp = 1; // peg to 30fps?
            // *gpuTimeStamp = 1000000; // 20fps everywhere but shrines?
        }
    };

    void InstallHooks() {
        EngineFrameworkGpuHook::Install();
    }
    */

} // ns

