
// WARNING: These are not vanilla/pure tas, they modify values in ram!
// Hacks to adjust movement direction independent of camera/etc
PlayerUtil::doLStickAbsoluteVanilla(); // nop
PlayerUtil::doLStickAbsoluteRadOffset(PI); // up is north (default arg)
PlayerUtil::doLStickAbsoluteTargetPos(Vector3f(26, 0, 1500));
PlayerUtil::doLStickAbsoluteTargetActorWatcher(1); // up is toward ActorWatcher[i].pos
PlayerUtil::doLStickAbsoluteCameraFreeze(); // up is where the camera is pointed
// (interferes with aim turn, dive turn, ...)
PlayerUtil::doLStickAbsolutePlayer(); // up is Player forward

