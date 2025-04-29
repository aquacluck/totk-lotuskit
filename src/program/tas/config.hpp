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
    };
    extern FrameAdvanceMode frameAdvanceMode;

} // ns

