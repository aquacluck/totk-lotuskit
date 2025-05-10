#pragma once
#include "exlaunch.hpp"

namespace lotuskit::tas::config {

    // how inputs should be dumped, and how playback commands should be interpreted
    enum class InputDurationScalingStrategy {
        FPS60_1X = 1, // awkward but 1:1: count in raw 30fps half-frames
        FPS30_2X = 2, // default: 1 frame = 30fps intended
        FPS20_3X = 3, // awkward and useless? assume we're pegged at 20fps and this is the new normal
    };
    extern InputDurationScalingStrategy inputMode;

    enum class VFRCopingStrategy {
        NULL_VANILLA = 0, // default: the game runs how it runs, don't try to adjust to anything or change anything
        // NULL_BONUS_N, // maybe "noninvasive" strategies to detect+reduce+react+debug+??? to low framerates?
        // FPS30_PEGGED, // peg to 30fps: the game will slowdown/stutter if it can't be achieved, but framecount based timing should behave more consistently
        // FPS20_PEGGED, // peg to 20fps: one possibility to simulate lag/etc
        // FPS60_PEGGED, // lol
        // FPS120_PEGGED, // lmao
        // TODO additional lag-inducing/lag-timing hacks?
    };
    extern VFRCopingStrategy vfrMode;

    enum class PlaybackInputPassthroughMode {
        NULL_VANILLA = 0,      // full passthrough ("sleep" or human only)
        PLAYBACK_TAS_ONLY = 1, // input supplied verbatim by tas
        PASSTHROUGH_OR = 2,    // human inputs added to tas inputs: human may add buttons or alter axes
        PASSTHROUGH_XOR = 3,   // human inputs invert tas inputs: human may toggle buttons or cancel out axes
    };
    extern PlaybackInputPassthroughMode playbackInputPassthroughMode; // TODO extract to ModuleStackFrameScheduleState

    /*
    enum class FrameAdvanceMode {
        NULL_VANILLA = 0, // game runs uninterrupted (unless paused manually / via user script)
        MOMENTARY_UNPAUSE = 1, // game is held in DebugPause by default, game will always return to DebugPause asap. briefly unpause with hotkey/ws/etc/tbd
        // INTERVAL_UNPAUSE = 2, // TODO game is held in DebugPause, pause-n unpause-m loop
        // PRACTICE = 3, // TODO controlled execution for human practice: various setup, then game is unpaused upon countdown/input/idk and control granted. can reset on various end conditions.
                         // might need separate AS ctx to poll/invoke on the side:
                         // ASClass IUserPracticeSpec { // user script implements:
                         //     void setup(); // eg load a savestate file
                         //     begintype isBegin(u64 buttons_for_hotkeyish); // run every frame, enum class begintype { wait_paused, wait_unpaused, abort, countdown, now }
                         //     bool isReset(); // run every frame, decides when to reset/quit/whatever
                         // }

        PAUSE_SKIPPED, // (default) script does not run during DebugPause -- but then how to briefly resume?
                       // isPlaybackBeginPending? but then we must know not to resume sp-1 ctx, and any hotkeyed input()s would also fail to resume when it might be expected.
                       // Perhaps we must opt-out of this per sp, and hotkeys+interrupts just use PAUSE_CONSUMES_FRAMES by default/option.
                       // MOMENTARY_UNPAUSE: the hotkey/interrupt ctx uses PAUSE_CONSUMES_FRAMES for its duration and proceeds as below -- but this is only the interrupting ctx which gets to run.
                       // How to briefly resume the *underlying sp-1 ctx?* we need a global framecount for momentary unpause. (global because per sp means it can overflow sp runlength and needs to get inherited back down the stack, very weird.)
                       // So we can just deduct these frames to bypass PAUSE_SKIPPED mode... these frames should also deduct from the resumed tas schedule of course.
                       // What happens when momentaryUnpauseFramesRemaining > 0 in PAUSE_CONSUMES_FRAMES mode? Leave momentaryUnpauseFramesRemaining alone so it only affects PAUSE_SKIPPED (this would be good because otherwise manual interrupts would consume these frames and fuck up any preexisting unpause schedule), or deduct in all modes?

        PAUSE_CONSUMES_FRAMES, // script does not stay suspended during DebugPause -- paused frames count for frametime calc, and script can explicitly unpause+step through game frames
                               // MOMENTARY_UNPAUSE: pause::releasePause("DebugPause"); tas::sleep(1); pause::requestPause("DebugPause"); // run a game frame with human input


    };
    */
    //extern FrameAdvanceMode frameAdvanceMode;

    // this all means
    // - we don't need global FrameAdvanceMode yet if ever
    // - ModuleStackFrameScheduleState needs a member enum DebugPauseMode { DEFAULT_SKIP_SCHEDULE=0, CONSUME_FRAMETIME=1 }
    // - we need global u32 schedule::tas::momentaryDebugUnpauseFramesRemaining which will unblock DEFAULT_SKIP_SCHEDULE mode sp while DebugPause
    // - (INTERVAL_UNPAUSE would need special logic to hold DebugPause and increment momentaryDebugUnpauseFramesRemaining 0->n after m pause time. Or for interactive use skip this, just CONSUME_FRAMETIME and explicitly manage pause.)
    // - (PRACTICE tbd)

} // ns

