pause::doTextWriter(true); // enable pause logging
pause::doTextWriterExtended(true); // extended data

pause::requestPause("LoadingPause");
pause::releasePause("LoadingPause");

pause::freezeTarget("Custom_PhysicsUpdateWorld"); // hold 1 target paused, wiping previous flags
pause::freezeTarget("Custom_PhysicsUpdateWorld", false, false); // hold 1 target unpaused, preserving previous flags
pause::freezeMask(0, 0, 0, 0); // hold all targets unpaused
pause::freezeTargetIndex(33);
pause::doFreezeMask(true); // freeze current pause state
pause::doFreezeMask(false); // stop freeze

