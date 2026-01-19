#include "exlaunch.hpp"
#include "structs/VFRMgr.hpp"
#include "util/fps.hpp"
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

        lotuskit::TextWriter::printf(1, "rta(fps, 1s) %.2f %.2f\nvfr(fps, 1s) %.2f %.2f\n", instTickFps, avgTickFps, instVFRFps, avgVFRFps);
    }

    void wsAnnounceConfig() {
        Logger::logJson(json::object({
            {"doTextWriter", doTextWriter},
        }), "/fps/log", false, false); // noblock, no debug log
    }

} // ns

