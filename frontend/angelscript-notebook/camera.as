camera::log(); // request camera dump
camera::toggleFreeze(); // freeze camera in place, or unfreeze

// gross LookAt setter: Vector3f up, Vector3d pos, Vector3d at
// pos is camera location, at is the camera target
camera::freeze(float, float, float, double, double, double, double, double, double);

//                      slot_i,  dist,       direction,   elev, follow, relative
camera::freezeAtActorWatcher(0, 10.69, Vector3f::NORTH, 20.420,   true, true);

