#include "exlaunch.hpp"
#include "util/world.hpp"

namespace lotuskit::util::world {
    void fogDisable_HACK_hook() {
        // ty zeldar
        __asm("FMOV S7, S0");
        __asm("FMOV S0, %s0" : : "w" (doHackFogDisable ? 0.0f : 1.0f)); // XXX wtf is this colon business
        __asm("FMOV S4, S0");
        __asm("FMOV S0, S7");
    }
    /*
    HOOK_DEFINE_INLINE(fogDisable_HACK_hook) {
        // ty zeldar
        static constexpr auto s_name = "game::wm::WorldManagerModule::fogDisable_HACK";
        static void Callback(exl::hook::InlineFloatCtx* ctx) {
            if (doHackFogDisable) { ctx->S[4] = 0.0f; } // XXX doesnt work?
            // else { assert(ctx->S[4] == 1.0f); } // vanilla
        }
    };
    */

    void InstallHooks() {
        exl::patch::CodePatcher(EXL_SYM_OFFSET("game::wm::WorldManagerModule::fogDisable_HACK")).BranchLinkInst((void*)fogDisable_HACK_hook);
    }

    void* getWorldManagerModuleInstance() {
        return *EXL_SYM_RESOLVE<void**>("game::wm::WorldManagerModule::sInstance");
    }

    void setFogDensity(float fogDensity, float fogNear, float fogFar, bool setFlag) {
        using impl_t = void (float, float, float, void*, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("game::wm::WorldManagerModule::setFogDensity");
        return impl(fogDensity, fogNear, fogFar, getWorldManagerModuleInstance(), setFlag);
    }

    void setWeather(u32 weather, bool setFlag) {
        using impl_t = void (void*, u32, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("game::wm::WorldManagerModule::setWeather");
        return impl(getWorldManagerModuleInstance(), weather, setFlag);
    }

    void setTime(float time) { // setFlag mWorldBaseParam[uVar1].mFlags | 0x20000000 always set in here
        using impl_t = void (float, void*);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("game::wm::WorldManagerModule::setTime");
        return impl(time, getWorldManagerModuleInstance());
    }

    void setTimeHM(float hours, float minutes) {
        setTimeDHMS(0, hours, minutes, 0);
    }

    void setTimeDHMS(float days, float hours, float minutes, float seconds) {
        setTime(seconds + 60*minutes + 60*60*hours + 60*60*24*days);
    }

    void setWind(float azimuth, float elevation, float speed, bool isImmTrans) {
        using impl_t = void (float, float, float, void*, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("game::wm::WorldManagerModule::setWind");
        return impl(azimuth, elevation, speed, getWorldManagerModuleInstance(), isImmTrans);
    }

} // ns

