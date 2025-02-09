pause::doTextWriter(true); // enable pause logging
pause::doTextWriterExtended(true); // extended data

// reqs are groups of targets paused at once when semaphore > 0
pause::requestPause("LoadingPause"); // increment
pause::releasePause("LoadingPause"); // decrement

// freeze helpers continuously inject a given pause mask:
pause::doFreezeMask(true); // freeze current pause state
pause::doFreezeMask(false); // stop freeze

// hold 1 req's targets paused, wiping previous flags:
pause::freezeRequest("ReverseRecorderPause"); // value=true, clearOthers=true

// hold 1 req's targets unpaused, preserving other flags:
pause::freezeRequest("ReverseRecorderPause", false, false);

// freeze helpers for more granular manipulation:
pause::freezeMask(0, 0, 0, 0); // hold all targets unpaused

// hold 1 target paused, wiping previous flags:
pause::freezeTarget("Custom_PhysicsUpdateWorld"); // value=true, clearOthers=true

// hold 1 target unpaused, preserving previous flags:
pause::freezeTarget("Custom_PhysicsUpdateWorld", false, false);

// mask bit index is rtl within each u32, but overflows through args ltr:
pause::freezeTargetIndex( 0, true, true ); // freezeMask(1,0,0,0) equiv
pause::freezeTargetIndex(32, true, false); // stacks to freezeMask(1,1,0,0) equiv

