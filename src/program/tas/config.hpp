#pragma once
#include "exlaunch.hpp"

namespace lotuskit::tas::config {

    // how inputs should be dumped, and how playback commands should be interpreted
    enum class InputDurationScalingStrategy {
        FPS60_1X = 1, // awkward but 1:1: count in raw 30fps half-frames
        FPS30_2X = 2, // default: 1 frame = 30fps intended
        FPS20_3X = 3  // awkward and useless? assume we're pegged at 20fps and this is the new normal
    };
    extern InputDurationScalingStrategy inputMode;

    enum class VFRCopingStrategy {
        NULL_VANILLA = 0, // default: the game runs how it runs, don't try to adjust to anything or change anything
        // NULL_BONUS_N, // maybe "noninvasive" strategies to detect+reduce+react+debug+??? to low framerates?
        // FPS30_PEGGED, // peg to 30fps: the game will slowdown/stutter if it can't be achieved, but framecount based timing should behave more consistently
        // FPS20_PEGGED // peg to 20fps: one possibility to simulate lag/etc
        // TODO additional lag-inducing/lag-timing hacks?
    };
    extern VFRCopingStrategy vfrMode;

} // ns

