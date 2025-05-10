#include <nn/util.h>
#include "tas/config.hpp"
#include "tas/Playback.hpp"
#include "script/schedule.hpp"
#include "structs/VFRMgr.hpp"
#include "util/hash.hpp"
#include "util/pause.hpp"
#include "TextWriter.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;
constexpr auto murmur32 = lotuskit::util::hash::murmur32;

namespace lotuskit::tas::Playback {
    void calc() {
        lotuskit::script::schedule::tas::abortStackImpl(); // stop+cleanup everything when pending abort. We do this outside script execution because... its very confusing lol.
                                                           // (AS ctx does not immediately suspend/etc when you ask it to from inside script execution)

        if (!lotuskit::script::schedule::tas::isPlaybackActive && !lotuskit::script::schedule::tas::isPlaybackBeginPending) {
            return; // not doing playback
        }

        // int 2 or 3 = float 1.0 (@30fps) or 1.5 (@20fps)
        // assert(mDeltaFrame == 1.0 || mDeltaFrame == 1.5); // precisely
        VFRMgr* vfrMgr = *EXL_SYM_RESOLVE<VFRMgr**>("engine::module::VFRMgr::sInstance");
        const u32 deltaFrame60 = (u32)(vfrMgr->mDeltaFrame * 2);

        const bool isCompleteOrFail = lotuskit::script::schedule::tas::calcCtx(deltaFrame60);
        if (isCompleteOrFail) { lotuskit::script::schedule::tas::isPlaybackActive = false; }
        drawTextWriterModeLine();
    }

    bool applyCurrentInput(nn::hid::NpadBaseState* dst) {
        if (!lotuskit::script::schedule::tas::isPlaybackActive) { return false; }
        const auto& state = lotuskit::script::schedule::tas::getSP()->state;

        switch(config::playbackInputPassthroughMode) {
            case config::PlaybackInputPassthroughMode::NULL_VANILLA:
            // passthrough all, do not alter input ("sleep")
            break;

            case config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY:
            std::memcpy((void*)&(dst->mButtons), (void*)&(state.input.buttons), 24);
            break;

            case config::PlaybackInputPassthroughMode::PASSTHROUGH_OR:
            *((u64*)&(dst->mButtons)) |= *((u64*)&(state.input.buttons));
            // passthrough all non-zero axes
            dst->mAnalogStickL.mX = (dst->mAnalogStickL.mX != 0) ? dst->mAnalogStickL.mX : state.input.LStick.mX;
            dst->mAnalogStickL.mY = (dst->mAnalogStickL.mY != 0) ? dst->mAnalogStickL.mY : state.input.LStick.mY;
            dst->mAnalogStickR.mX = (dst->mAnalogStickR.mX != 0) ? dst->mAnalogStickR.mX : state.input.RStick.mX;
            dst->mAnalogStickR.mY = (dst->mAnalogStickR.mY != 0) ? dst->mAnalogStickR.mY : state.input.RStick.mY;
            // FIXME axis button flags not updated
            break;

            case config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR:
            *((u64*)&(dst->mButtons)) ^= *((u64*)&(state.input.buttons));
            // toggle all non-zero axes
            dst->mAnalogStickL.mX = (state.input.LStick.mX == 0) ? dst->mAnalogStickL.mX :
                                    ( dst->mAnalogStickL.mX == 0) ? state.input.LStick.mX : 0;
            dst->mAnalogStickL.mY = (state.input.LStick.mY == 0) ? dst->mAnalogStickL.mY :
                                    ( dst->mAnalogStickL.mY == 0) ? state.input.LStick.mY : 0;
            dst->mAnalogStickR.mX = (state.input.RStick.mX == 0) ? dst->mAnalogStickR.mX :
                                    ( dst->mAnalogStickR.mX == 0) ? state.input.RStick.mX : 0;
            dst->mAnalogStickR.mY = (state.input.RStick.mY == 0) ? dst->mAnalogStickR.mY :
                                    ( dst->mAnalogStickR.mY == 0) ? state.input.RStick.mY : 0;
            // FIXME axis button flags not updated
            break;
        }
        return true;
    }

    bool applyCurrentGyro(nn::hid::SixAxisSensorState* dst_gyro) {
        if (!lotuskit::script::schedule::tas::isPlaybackActive) { return false; }
        const auto& state = lotuskit::script::schedule::tas::getSP()->state;

        switch(config::playbackInputPassthroughMode) {
            case config::PlaybackInputPassthroughMode::NULL_VANILLA:
            // passthrough all, do not alter input ("sleep")
            break;

            case config::PlaybackInputPassthroughMode::PASSTHROUGH_OR: // gyro unsupported
            case config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR: // gyro unsupported
            case config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY:
            // XXX hack dont clobber sample header
            std::memcpy((void*)&(dst_gyro->linearAcceleration), (void*)&(state.input.gyro.linearAcceleration), sizeof(nn::hid::SixAxisSensorState) - 0x10);
            break;
        }
        return true;
    }

    // script binds for: pause scheduling
    void doScheduleAwaitPauseRequestStr(const std::string& requestKey) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;

        state.awaitPauseVal = true;
        state.awaitBegin60 = lotuskit::script::schedule::tas::elapsedPlayback60;
        state.isAwaitPauseRequestHash = murmur32(requestKey);

        lotuskit::script::schedule::tas::isPlaybackActive = true;
        lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
    }
    void doScheduleAwaitUnpauseRequestStr(const std::string& requestKey) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;

        state.awaitPauseVal = false;
        state.awaitBegin60 = lotuskit::script::schedule::tas::elapsedPlayback60;
        state.isAwaitPauseRequestHash = murmur32(requestKey);

        lotuskit::script::schedule::tas::isPlaybackActive = true;
        lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
    }
    void doScheduleAwaitPauseTargetStr(const std::string& targetKey) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;

        state.awaitPauseVal = true;
        state.awaitBegin60 = lotuskit::script::schedule::tas::elapsedPlayback60;
        state.isAwaitPauseTargetHash = murmur32(targetKey);

        lotuskit::script::schedule::tas::isPlaybackActive = true;
        lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
    }
    void doScheduleAwaitUnpauseTargetStr(const std::string& targetKey) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;

        state.awaitPauseVal = false;
        state.awaitBegin60 = lotuskit::script::schedule::tas::elapsedPlayback60;
        state.isAwaitPauseTargetHash = murmur32(targetKey);

        lotuskit::script::schedule::tas::isPlaybackActive = true;
        lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
    }
    void doSkipDebugPause(bool v) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.skipDebugPause = v;
    }
    void doSkipLoadingPause(bool v) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.skipLoadingPause = v;
    }

    // script binds for: current input
    void setCurrentInput(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        // called by tas script to specify input for next n frames
        auto& state = lotuskit::script::schedule::tas::getSP()->state;

        // what is "n frames" expected to mean?
        u32 duration60 = 0;
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            duration60 = duration;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            duration60 = duration * 2;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            duration60 = duration * 3;
        } else return;

        //TODO atomic swap for state.input?
        state.inputTTL60 = duration60;
        state.input.LStick.mX = nextLStickX;
        state.input.LStick.mY = nextLStickY;
        state.input.RStick.mX = nextRStickX;
        state.input.RStick.mY = nextRStickY;

        // rehydrate axis flags, matching 16400 sdk threshold (arguably bad for us, hiding low signals). idk why these are buttons but it's useful to preserve them for InputDisplay
        if (nextLStickX >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLRight); }
        if (nextLStickX < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLLeft); }
        if (nextLStickY >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLUp); }
        if (nextLStickY < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLDown); }
        if (nextRStickX >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRRight); }
        if (nextRStickX < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRLeft); }
        if (nextRStickY >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRUp); }
        if (nextRStickY < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRDown); }

        // TODO extract "playback begin" logic?
        if (!lotuskit::script::schedule::tas::isPlaybackActive) {
            lotuskit::script::schedule::tas::elapsedPlayback60 = 0;
        }

        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY;
        *(u64*)&(state.input.buttons) = nextButtons;              // These are expected to be
        lotuskit::script::schedule::tas::isPlaybackActive = true; // written/observable in order
        // XXX and i'm not confident that's guaranteed, but it doesnt seem dangerous at least?
        //     i do this sort of guard without locking a lot though, so i need to find out. std::memory_order or something?

        if (duration60 > 0) {
            lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game, to be resumed in n frames
        }
    }
    void setCurrentInputOr(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        setCurrentInput(duration, nextButtons, nextLStickX, nextLStickY, nextRStickX, nextRStickY);
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::PASSTHROUGH_OR; // TODO extract passthrough mode to sp state
    }
    void setCurrentInputXor(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        setCurrentInput(duration, nextButtons, nextLStickX, nextLStickY, nextRStickX, nextRStickY);
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR; // TODO extract passthrough mode to sp state
    }
    void setSleepInput(u32 duration) {
        // called by tas script to passthrough human input for next n frames
        setCurrentInput(duration, 0, 0,0, 0,0);
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::NULL_VANILLA; // TODO extract passthrough mode to sp state
    }

    // script binds for: current gyro input
    void setCurrentGyroLinearAcceleration(const sead::Vector3f& linearAcceleration) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.input.gyro.linearAcceleration = linearAcceleration;
    }
    void setCurrentGyroAngularVelocity(const sead::Vector3f& angularVelocity) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.input.gyro.angularVelocity = angularVelocity;
    }
    void setCurrentGyroAngularVelocitySum(const sead::Vector3f& angularVelocitySum) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.input.gyro.angularVelocitySum = angularVelocitySum;
    }
    void setCurrentGyroRotation(const sead::Matrix33f& rotation) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.input.gyro.rotation = rotation;
    }
    void setCurrentGyroAll(const sead::Vector3f& linearAcceleration, const sead::Vector3f& angularVelocity, const sead::Vector3f& angularVelocitySum, const sead::Matrix33f& rotation) {
        auto& state = lotuskit::script::schedule::tas::getSP()->state;
        state.input.gyro.linearAcceleration = linearAcceleration;
        state.input.gyro.angularVelocity    = angularVelocity;
        state.input.gyro.angularVelocitySum = angularVelocitySum;
        state.input.gyro.rotation           = rotation;
    }
    void setCurrentGyroAllZero() {
        auto v = sead::Vector3f{0, 0, 0};
        auto m = sead::Matrix33f{0,0,0, 0,0,0, 0,0,0};
        setCurrentGyroAll(v, v, v, m);
    }

    void beginFrameAdvance() {
        //lotuskit::TextWriter::toastf(30, "beginFrameAdvance(%d)\n", 420); // XXX

        if (!lotuskit::script::schedule::tas::isFrameAdvance) {
            lotuskit::script::schedule::tas::isFrameAdvance = true;
            lotuskit::script::schedule::tas::bypassDebugPause60 = 0;
        }
        if (!lotuskit::util::pause::isPauseRequestStr("DebugPause")) {
            lotuskit::util::pause::requestPauseStr("DebugPause");
        }
    }

    void stepFrameAdvance(u32 duration) {
        //lotuskit::TextWriter::toastf(30, "stepFrameAdvance(%d)\n", duration); // XXX

        u32 duration60 = 0;
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            duration60 = duration;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            duration60 = duration * 2;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            duration60 = duration * 3;
        }
        if (!lotuskit::script::schedule::tas::isFrameAdvance) {
            beginFrameAdvance();
        }
        lotuskit::script::schedule::tas::bypassDebugPause60 += duration60;
    }

    void endFrameAdvance() {
        //lotuskit::TextWriter::toastf(30, "endFrameAdvance(%d)\n", 420); // XXX

        lotuskit::util::pause::releasePauseStr("DebugPause"); // XXX might be easier to just force 0 and 1 instead of inc/dec
        lotuskit::script::schedule::tas::bypassDebugPause60 = 0;
        lotuskit::script::schedule::tas::isFrameAdvance = false;
    }

    void drawTextWriterModeLine() {
        const auto& state = lotuskit::script::schedule::tas::getSP()->state;
        const auto currentInputTTL60 = state.inputTTL60;
        const auto elapsedPlayback60 = lotuskit::script::schedule::tas::elapsedPlayback60;
        // TODO opts for system tick? this frame's delta time? rta? igt?
        // (however its impossible to predict or progress-bar a script's frame duration -- halting problem)
        if (!lotuskit::script::schedule::tas::isPlaybackActive) {
            lotuskit::TextWriter::printf(1, "tas::end TOTK_%d\n               fr:%6d\n",                                                 TOTK_VERSION, duration60ToUIFrames(elapsedPlayback60));
        } else if (config::playbackInputPassthroughMode == config::PlaybackInputPassthroughMode::NULL_VANILLA) {
            lotuskit::TextWriter::printf(1, "tas::sleep(%3u) TOTK_%d\n               fr:%6d\n", duration60ToUIFrames(currentInputTTL60), TOTK_VERSION, duration60ToUIFrames(elapsedPlayback60));
        } else if (config::playbackInputPassthroughMode == config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY) {
            lotuskit::TextWriter::printf(1, "tas::input(%3u) TOTK_%d\n               fr:%6d\n", duration60ToUIFrames(currentInputTTL60), TOTK_VERSION, duration60ToUIFrames(elapsedPlayback60));
        } else if (config::playbackInputPassthroughMode == config::PlaybackInputPassthroughMode::PASSTHROUGH_OR) {
            lotuskit::TextWriter::printf(1, "   inputOr(%3u) TOTK_%d\n               fr:%6d\n", duration60ToUIFrames(currentInputTTL60), TOTK_VERSION, duration60ToUIFrames(elapsedPlayback60));
        } else if (config::playbackInputPassthroughMode == config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR) {
            lotuskit::TextWriter::printf(1, "  inputXor(%3u) TOTK_%d\n               fr:%6d\n", duration60ToUIFrames(currentInputTTL60), TOTK_VERSION, duration60ToUIFrames(elapsedPlayback60));
        }
    }

    u32 duration60ToUIFrames(u32 duration60) {
        // how many "frames" is the timespan
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            return duration60;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            return duration60 / 2;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            return duration60 / 3;
        }
        return 0xdeaddead;
    }

} // ns
