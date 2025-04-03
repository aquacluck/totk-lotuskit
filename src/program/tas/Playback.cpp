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

namespace lotuskit::tas {
    PlaybackInput Playback::currentInput = {0};

    /// pause scheduling {{{
        // TODO extract some interface to register awaitable calls?

        bool awaitPauseVal = false; // common: waiting for pause vs unpause
        u32 awaitBegin60 = 0; // common: for time spent waiting, relative to elapsedPlayback60
        u32 isAwaitPauseRequestHash = 0;
        u32 isAwaitPauseTargetHash = 0;

        bool Playback::calcScheduleIsAwaitPauseRequest() {
            if (isAwaitPauseRequestHash == 0) { return false; }
            const auto pauseMgr = lotuskit::util::pause::pauseMgr;
            const auto name = pauseMgr->getPauseRequestKey(isAwaitPauseRequestHash);
            const bool isPause = pauseMgr->getPauseRequestCount(isAwaitPauseRequestHash) > 0;
            const bool ret = awaitPauseVal != isPause;
            if (ret) {
                // TODO display pause name
                lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
                    textPos->x = 1280.0 - 350.0;
                });
                if (awaitPauseVal) {
                    lotuskit::TextWriter::printf(1, "awaitPauseRequest(%s:%3u)\n               fr:%6d\n", name, duration60ToUIFrames(elapsedPlayback60-awaitBegin60), duration60ToUIFrames(elapsedPlayback60));
                } else {
                    lotuskit::TextWriter::printf(1, "awaitUnpauseRequest(%s:%3u)\n               fr:%6d\n", name, duration60ToUIFrames(elapsedPlayback60-awaitBegin60), duration60ToUIFrames(elapsedPlayback60));
                }
            } else {
                isAwaitPauseRequestHash = 0; // end await
            }
            return ret;
        }
        void Playback::doScheduleAwaitPauseRequestStr(const std::string& requestKey) {
            awaitPauseVal = true;
            awaitBegin60 = elapsedPlayback60;
            isAwaitPauseRequestHash = murmur32(requestKey);

            isPlaybackActive = true;
            lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
        }
        void Playback::doScheduleAwaitUnpauseRequestStr(const std::string& requestKey) {
            awaitPauseVal = false;
            awaitBegin60 = elapsedPlayback60;
            isAwaitPauseRequestHash = murmur32(requestKey);

            isPlaybackActive = true;
            lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
        }

        bool Playback::calcScheduleIsAwaitPauseTarget() {
            if (isAwaitPauseTargetHash == 0) { return false; }
            const auto pauseMgr = lotuskit::util::pause::pauseMgr;
            const auto name = pauseMgr->getPauseTargetKey(isAwaitPauseTargetHash);
            const bool isPause = pauseMgr->isTargetPaused(isAwaitPauseTargetHash);
            const bool ret = awaitPauseVal != isPause;
            if (ret) {
                lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
                    textPos->x = 1280.0 - 350.0;
                });
                if (awaitPauseVal) {
                    lotuskit::TextWriter::printf(1, "tas::awaitPauseTarget(%s:%3u)\n               fr:%6d\n", name, duration60ToUIFrames(elapsedPlayback60-awaitBegin60), duration60ToUIFrames(elapsedPlayback60));
                } else {
                    lotuskit::TextWriter::printf(1, "tas::awaitUnpauseTarget(%s:%3u)\n               fr:%6d\n", name, duration60ToUIFrames(elapsedPlayback60-awaitBegin60), duration60ToUIFrames(elapsedPlayback60));
                }
            } else {
                isAwaitPauseTargetHash = 0; // end await
            }
            return ret;
        }
        void Playback::doScheduleAwaitPauseTargetStr(const std::string& targetKey) {
            awaitPauseVal = true;
            awaitBegin60 = elapsedPlayback60;
            isAwaitPauseTargetHash = murmur32(targetKey);

            isPlaybackActive = true;
            lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
        }
        void Playback::doScheduleAwaitUnpauseTargetStr(const std::string& targetKey) {
            awaitPauseVal = false;
            awaitBegin60 = elapsedPlayback60;
            isAwaitPauseTargetHash = murmur32(targetKey);

            isPlaybackActive = true;
            lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game
        }
    /// }}} pause scheduling

    void Playback::calc() {
        lotuskit::script::schedule::tas::abortStackImpl(); // stop+cleanup everything when pending abort. We do this outside script execution because... its very confusing lol.
                                                           // (ctxs do not immediately suspend/etc when you ask them to during script execution)
        if (!isPlaybackActive && !isExecuteCtxRequested) { return; } // not doing playback
        if (isExecuteCtxRequested) {
            // bypass async scheduling -- FIXME async flags should all be isolated per stackframe and
            // cleared on enter/push so the remainder of function below will flow straight through
            // on deferred module's first run instead of this bypass.
            isExecuteCtxRequested = false;
            calcAS();
            return;
        }

        const bool isLoadingPause = lotuskit::util::pause::isPauseRequest(0x0eafe200);
        if (isLoadingPause && skipLoadingPause) {
            if (isAwaitPauseRequestHash == 0x0eafe200 && awaitPauseVal == true) {
                // do not resume, but if script is awaiting LoadingPause request, clear the blocking condition
                isAwaitPauseRequestHash = 0;
            }
            lotuskit::TextWriter::printf(1, "tas::nop(LoadingPause)\n               fr:%6d\n", duration60ToUIFrames(elapsedPlayback60));
            return;
        }

        { // begin frametime scheduling // TODO extract `bool calcScheduleIsScriptInputExhausted()`
            // int 2 or 3 = float 1.0 (@30fps) or 1.5 (@20fps)
            // assert(mDeltaFrame == 1.0 || mDeltaFrame == 1.5); // precisely
            VFRMgr* vfrMgr = *EXL_SYM_RESOLVE<VFRMgr**>("engine::module::VFRMgr::sInstance");
            const u32 deltaFrame60 = (u32)(vfrMgr->mDeltaFrame * 2);

            // should the current input state still be scheduled?
            if (currentInputTTL60 >= deltaFrame60) {
                // deduct frames from schedule
                currentInputTTL60 -= deltaFrame60;
                elapsedPlayback60 += deltaFrame60;
            } else {
                // only 0/1/2 frames60 left on schedule and deltaFrame60 is larger
                // XXX is erring on one side or the other better, until i properly figure this out?
                //     ie playing out these closing frames and deferring or eating into subsequent inputs? sounds complicated in the dumb way
                currentInputTTL60 = 0; // abandon/truncate the remainder of this scheduled input and resume script
                elapsedPlayback60 += deltaFrame60; // desync observable by user when elapsed time exceeds their expected schedule
            }

            if (currentInputTTL60 > 0) {
                return Playback::drawTextWriterModeLine(); // keep using current scheduled input
            }

            if (!lotuskit::script::schedule::tas::hasPlayableCtx()) {
                // XXX is it better to repeat the last input or drop out? this has never happened yet
                Logger::logText("[error] invalid tas ctx, cannot advance input", "/tas/Playback");
                isPlaybackActive = false; // XXX if its "not playable" then lets stop playing hmm?
                return Playback::drawTextWriterModeLine();
            }
        } // end frametime scheduling

        // pause scheduling
        if (Playback::calcScheduleIsAwaitPauseRequest()) { return; } // modeline drawn on wait
        if (Playback::calcScheduleIsAwaitPauseTarget())  { return; } // modeline drawn on wait

        calcAS(); // resume script, eventually getting a setCurrentInput call
    }

    void Playback::calcAS() {
        // execute AS stack until it yields or completes
        const bool isCompleteOrFail = lotuskit::script::schedule::tas::calcCtx();
        if (isCompleteOrFail) { isPlaybackActive = false; }
        Playback::drawTextWriterModeLine();
    }

    void Playback::drawTextWriterModeLine() {
        // TODO opts for system tick? this frame's delta time? rta? igt?
        // (however its impossible to predict or progress-bar a script's frame duration -- halting problem)
        if (!isPlaybackActive) {
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

    void Playback::setCurrentInput(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        // called by tas script to specify input for next n frames

        // what is "n frames" expected to mean?
        u32 duration60 = 0;
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            duration60 = duration;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            duration60 = duration * 2;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            duration60 = duration * 3;
        } else return;

        //TODO atomic toggle between double buffer currentInput+nextInput
        currentInputTTL60 = duration60;
        currentInput.LStick.mX = nextLStickX;
        currentInput.LStick.mY = nextLStickY;
        currentInput.RStick.mX = nextRStickX;
        currentInput.RStick.mY = nextRStickY;

        // rehydrate axis flags, matching 16400 sdk threshold (arguably bad for us, hiding low signals). idk why these are buttons but it's useful to preserve them for InputDisplay
        if (nextLStickX >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLRight); }
        if (nextLStickX < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLLeft); }
        if (nextLStickY >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLUp); }
        if (nextLStickY < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickLDown); }
        if (nextRStickX >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRRight); }
        if (nextRStickX < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRLeft); }
        if (nextRStickY >  16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRUp); }
        if (nextRStickY < -16400) { nextButtons |= (1 << (u32)nn::hid::NpadButton::StickRDown); }

        if (!isPlaybackActive) { elapsedPlayback60 = 0; }
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY;
        *(u64*)&(currentInput.buttons) = nextButtons; // These are expected to be
        isPlaybackActive = true;                      // written/observable in order
        // XXX and i'm not confident that's guaranteed, but it doesnt seem dangerous at least?
        //     i do this sort of guard without locking a lot though, so i need to find out. std::memory_order or something?

        isAwaitPauseTargetHash = 0; // clear any awaits TODO extract
        isAwaitPauseRequestHash = 0;
        if (duration60 > 0) {
            lotuskit::script::schedule::tas::trySuspendCtx(); // yield execution from script back to game, to be resumed in n frames
        }
    }

    void Playback::setCurrentInputOr(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        Playback::setCurrentInput(duration, nextButtons, nextLStickX, nextLStickY, nextRStickX, nextRStickY);
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::PASSTHROUGH_OR;
    }

    void Playback::setCurrentInputXor(u32 duration, u64 nextButtons, s32 nextLStickX, s32 nextLStickY, s32 nextRStickX, s32 nextRStickY) {
        Playback::setCurrentInput(duration, nextButtons, nextLStickX, nextLStickY, nextRStickX, nextRStickY);
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR;
    }

    void Playback::setSleepInput(u32 duration) {
        // called by tas script to passthrough human input for next n frames
        Playback::setCurrentInput(duration, 0, 0,0, 0,0);
        config::playbackInputPassthroughMode = config::PlaybackInputPassthroughMode::NULL_VANILLA;
    }

    bool Playback::applyCurrentInput(nn::hid::NpadBaseState* dst) {
        if (!isPlaybackActive) { return false; }
        switch(config::playbackInputPassthroughMode) {
            case config::PlaybackInputPassthroughMode::NULL_VANILLA:
            // passthrough all, do not alter input ("sleep")
            break;

            case config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY:
            std::memcpy((void*)&(dst->mButtons), (void*)&(currentInput.buttons), 24);
            break;

            case config::PlaybackInputPassthroughMode::PASSTHROUGH_OR:
            *((u64*)&(dst->mButtons)) |= *((u64*)&(currentInput.buttons));
            // passthrough all non-zero axes
            dst->mAnalogStickL.mX = (dst->mAnalogStickL.mX != 0) ? dst->mAnalogStickL.mX : currentInput.LStick.mX;
            dst->mAnalogStickL.mY = (dst->mAnalogStickL.mY != 0) ? dst->mAnalogStickL.mY : currentInput.LStick.mY;
            dst->mAnalogStickR.mX = (dst->mAnalogStickR.mX != 0) ? dst->mAnalogStickR.mX : currentInput.RStick.mX;
            dst->mAnalogStickR.mY = (dst->mAnalogStickR.mY != 0) ? dst->mAnalogStickR.mY : currentInput.RStick.mY;
            // FIXME axis button flags not updated
            break;

            case config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR:
            *((u64*)&(dst->mButtons)) ^= *((u64*)&(currentInput.buttons));
            // toggle all non-zero axes
            dst->mAnalogStickL.mX = (currentInput.LStick.mX == 0) ? dst->mAnalogStickL.mX :
                                    ( dst->mAnalogStickL.mX == 0) ? currentInput.LStick.mX : 0;
            dst->mAnalogStickL.mY = (currentInput.LStick.mY == 0) ? dst->mAnalogStickL.mY :
                                    ( dst->mAnalogStickL.mY == 0) ? currentInput.LStick.mY : 0;
            dst->mAnalogStickR.mX = (currentInput.RStick.mX == 0) ? dst->mAnalogStickR.mX :
                                    ( dst->mAnalogStickR.mX == 0) ? currentInput.RStick.mX : 0;
            dst->mAnalogStickR.mY = (currentInput.RStick.mY == 0) ? dst->mAnalogStickR.mY :
                                    ( dst->mAnalogStickR.mY == 0) ? currentInput.RStick.mY : 0;
            // FIXME axis button flags not updated
            break;
        }
        return true;
    }

    bool Playback::applyCurrentGyro(nn::hid::SixAxisSensorState* dst_gyro) {
        if (!isPlaybackActive) { return false; }
        switch(config::playbackInputPassthroughMode) {
            case config::PlaybackInputPassthroughMode::NULL_VANILLA:
            // passthrough all, do not alter input ("sleep")
            break;

            case config::PlaybackInputPassthroughMode::PASSTHROUGH_OR: // gyro unsupported
            case config::PlaybackInputPassthroughMode::PASSTHROUGH_XOR: // gyro unsupported
            case config::PlaybackInputPassthroughMode::PLAYBACK_TAS_ONLY:
            // XXX hack dont clobber sample header
            std::memcpy((void*)&(dst_gyro->linearAcceleration), (void*)&(currentInput.gyro.linearAcceleration), sizeof(nn::hid::SixAxisSensorState) - 0x10);
            break;
        }
        return true;
    }

    u32 Playback::duration60ToUIFrames(u32 duration60) {
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
