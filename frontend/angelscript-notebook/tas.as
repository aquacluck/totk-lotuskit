// do nothing for 1 second or 30 ideal frames
// (when using default "fixed 30fps" scale)
tas::input(30, NONE, 0,0, 0,0);

// tap dpad down and B button for 1 ideal frame,
// and hold rstick left just far enough to trigger input display
// (chosen because abs(x) > 16400 is when nnsdk flags it as a button press)
tas::input(1, KEY_DDOWN|KEY_B, 0,0, -16401,0);

// or use vector math: LStick: 0,16383, RStick: -8000,0
tas::input(15, KEY_B, 0.5*STICK_UP_MAX, 8000*STICK_LEFT);
tas::input(30, STICK_ZERO, STICK_ZERO); // any args may be omitted

tas::sleep(30); // passthrough human control for 30f
tas::inputOr(30, KEY_ZL); // OR human controls together with script
tas::inputXor(30, KEY_ZR); // XOR human controls together with script

// TODO angle+arc+tweening helpers

// input recording/dumping
tas::beginFileDump("sdcard:/totk_lotuskit/name_me.as");
tas::beginFileDumpNXTas("sdcard:/totk_lotuskit/out.txt");
tas::beginLoggerDump(false); // disable ws
tas::beginLoggerDumpNXTas();
tas::toggleDump(); // reuse latest settings
tas::endDump();


// Local files are loaded+unloaded as needed, so you can
// organize extremely long scripts in a hierarchy/etc:
tas::awaitExecFile("sdcard:/totk_lotuskit/name_me.as");
tas::abort("rip"); // end current script, optional err
tas::dumpStack(); // print files on tas stack, max depth 12
// Convert and exec frame-per-line `0 NONE 0;0 0;0` format
tas::awaitExecFileNXTas("sdcard:/totk_lotuskit/script0-1.txt");
tas::awaitEval("void main() { TextWriter::print(0, \"wow\n\"); }");

/* tas stack caveats:
 - Files don't share variables, call shared functions,
   or directly pass arguments. They run in isolation
   (to simplify memory management), however they can all
   access the same game data with the same AS bindings.
 - Each file/source is built once per script run,
   it cannot be modified during that AS execution.
   (FIXME "leaf" files do get rebuilt, this is bad for perf)
 - FIXME Interrupting waits/inputs with more waits/inputs
   will overwrite those timers/flags/etc, as this state is
   currently shared across all files. (This applies to eg
   buttons and hotkeys which actively interrupt the script.
   Nesting tas::awaitExecFile() itself is fine because it
   runs inline/between those calls, never interrupting them)
*/


/***********************************************************
/* About tas::input time scaling:
/*
/* Input durations are measured in 2 * desired 30fps frames
/* internal to lotuskit, which you can also think of as a fake 60fps:
/* 1 frame @ 30fps = 2 @ 60fps
/*
/* VFRMgr will toggle between 20fps and 30fps effective speed,
/* so counting half-frames means we can still use whole numbers.
/* 3 frames @ 60fps = 1 @ 20fps
/*
/* By default this behavior is hidden -- dump+playback both scale
/* by 2 and drop remainders in inputMode FPS30_2X, so unless you
/* encounter a lot of lag you can pretend it's straight 30fps.
/*
/* TODO AS access to set InputDurationScalingStrategy
/* TODO implement VFRCopingStrategy::FPS30_PEGGED
/**********************************************************/


/*
void tas::input(
     u32 duration=1, u64 nextButtons=0,
     s32 nextLStickX=0, s32 nextLStickY=0,
     s32 nextRStickX=0, s32 nextRStickY=0
);

All the supported button codes:
NONE
KEY_A
KEY_B
KEY_X
KEY_Y
KEY_L
KEY_R
KEY_ZL
KEY_ZR
KEY_PLUS
KEY_MINUS
KEY_DLEFT
KEY_DUP
KEY_DRIGHT
KEY_DDOWN
KEY_LSTICK
KEY_RSTICK
*/


/***********************************************************
/* Considerations for mixing tas::input() with expensive scripting
/*
/* Upon execution, `tas::input` writes to a "current input" buffer
/* which feeds a hook on the game's requests to the sdk for input states.
/* Then your script yields, and that single input gets injected forever
/* until your script changes it to something else.
/*
/* Now your script is *completely paused*, while the game advances through
/* the frames you yielded. The mod resumes your script after this time,
/* and you *may* change it again immediately with another `tas::input`.
/*
/* If you do anything else now (any work which might take time or block),
/* the old input is still there, and this is a multithreaded game, so
/* your busy script won't stop other threads from observing it.
/*
/* So if you're doing expensive work while still trying for high input
/* accuracy, consider that this work may extend the previous state.
/* (see [sys.as] for more info on yields and scheduling)
/*
/* Note that socket io and script execution are not offloaded to
/* threads. Writes may be deferred, and reads poll instead of block,
/* but the work is still done in the "main" WorldManagerModule hook.
/**********************************************************/


void example_qds1() {
    // based on armindoemiya's qds instructions
    // https://discord.com/channels/1086729144307564648/1109838351596527726/1325295759674835109
    // assert inventory is weapon tab, cursor on equipped filler, target to left of cursor
    // assert unpaused, standing, back against wall, not overloaded
    tas::input(1, KEY_PLUS, 0,0, 0,0); // pause
    tas::input(2, KEY_DLEFT, 0,0, 0,0); // wait + cursor left to target
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // swap to target
    tas::input(3, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(1, NONE, 0,0, 0,0);
    tas::input(1, KEY_DDOWN, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // drop target
    tas::input(3, NONE, 0,0, 0,0);
    tas::input(1, KEY_PLUS, 0,0, 0,0); // unpause
    tas::input(3, NONE, 0,0, 0,0);
    tas::input(1, KEY_PLUS, 0,0, 0,0); // repause
    tas::input(1, NONE, 0,0, 0,0);
    // assert cursor is on equipped target
    // assert available filler to left of cursor
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(1, NONE, 0,0, 0,0);
    tas::input(1, KEY_DDOWN, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // drop target
    tas::input(1, NONE, 0,0, 0,0);
    tas::input(1, KEY_DLEFT, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // equip filler to left
    tas::input(3, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // unequip
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(1, KEY_PLUS, 0,0, 0,0); // unpause
    tas::input(3, NONE, 0,32767, 0,0); // up turnaround
    tas::input(1, KEY_PLUS, 0,0, 0,0); // repause
    tas::input(1, NONE, 0,0, 0,0);
    // assert cursor is on equipped target
    // assert available filler to left of cursor
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(1, NONE, 0,0, 0,0);
    tas::input(1, KEY_DDOWN, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // drop target
    tas::input(1, NONE, 0,0, 0,0);
    tas::input(1, KEY_DLEFT, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // equip filler to left
    tas::input(3, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0);
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(1, KEY_A, 0,0, 0,0); // unequip
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(1, KEY_PLUS, 0,0, 0,0); // unpause
    // manually turn around and take it, whatever
}


void example_cartslide1() {
    //actor::createSimple("Weapon_Sword_124"); // royal bsword
    // Weapon_Lsword_124 royal clay, SpObj_Cart_A_01, SpObj_Rocket_A_01
    //example_qds1(); // prereqs

    Player.setPosRot(-170, 437, -980, 1, 0, 0, 0, 1, 0, 0, 0, 1);
    tas::input(15, NONE); // TODO fix camera
    tas::input(15, KEY_ZL); // TODO fix camera

    // assert cart in quickmenu cursor, recall rune
    tas::input(6, KEY_DUP|KEY_ZL);
    tas::input(1, KEY_X|KEY_DUP|KEY_ZL);
    tas::input(14, KEY_ZL); // wait for drop

    // recall cart
    tas::input(1, KEY_L|KEY_ZL);
    tas::input(8, KEY_ZL, 0,0, 0,32767); //XXX inverted camera
    ActorWatcher::assignSlotAwaitRecall(1);
    tas::input(2, KEY_ZL); //XXX need to stop/slow cursor to select?
    tas::input(1, KEY_A|KEY_ZL); // begin recall
    tas::input(60, KEY_ZL); // wait for drop/origin position
    tas::input(1, KEY_X|KEY_ZL, 0,32767, 0,0); // jump on
    tas::input(30, KEY_ZL);
    tas::input(1, KEY_L|KEY_ZL); // end recall
}

// pause request+target based scheduling
tas::awaitPauseRequest("EventDisableController_Camera");
tas::awaitUnpauseRequest("EventDisableController_Camera");
tas::awaitPauseTarget("Custom_Footprint");
tas::awaitUnpauseTarget("Custom_Footprint");

// Keep running tas through specific pauses:
tas::doSkipDebugPause(false);
tas::doSkipLoadingPause(false);

// toggle input display
tas::doTextWriterInputDisplay(false);
tas::doTextWriterGyro(true);

// setting gyro does not schedule frames!
// you must call tas::input(n) to begin+continue tas playback
tas::setGyroLinearAcceleration(Vector3f linearAcceleration);
tas::setGyroAngularVelocity(Vector3f angularVelocity);
tas::setGyroAngularVelocitySum(Vector3f angularVelocitySum);
tas::setGyroRotation(Matrix33f rotation); // TODO tas::getGyro* getters
tas::clearGyro(); // set gyro fields to 0 so tas::input stops disrupting camera, ie:
tas::setGyroAll(Vector3f::ZERO, Vector3f::ZERO, Vector3f::ZERO, Matrix33f::ZERO);

// that is all :) ur doing great

