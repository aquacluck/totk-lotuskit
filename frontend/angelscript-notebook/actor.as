// spawned near Player by default
actor::createSimple("GameRomHorse");

// spawn at coordinates (engine coordinates, not ui)
actor::createSimple("GameRomHorse", 123, 456.0, 789.5);

// assign to ActorWatcher slot 1 on creation
actor::createAndWatch(1, "GameRomHorse");

// wait for actor to spawn before accessing
// (see [tas.as] for more info on timing)
tas::input(4, NONE);

// get a handle to the watched horse
ActorBase@ honse1 = ActorWatcher::get(1);

// some actors like Player have permanent handles
ActorWatcher::assignSlot(0, Player); // default slot
ActorWatcher::assignSlot(0, honse1);

// actor position can be read and written with pos_*
if (Player.pos_y < honse1.pos_y) {
    Player.pos_y = honse1.pos_y + 5;

    // or use vectors
    Player.pos = honse1.pos + Vector3f(0, 5, 0);
}

// spawn a wing below Player, watch it in slot 2
actor::createAndWatch(
    2, "SpObj_LiftGeneratorWing_A_01",
    Player.pos + 5*Vector3f::DOWN,
    Player.rot // inherit rotation
);

// or i do this a lot and it smacks Link hehe
actor::createAndWatch(
    1, "SpObj_LiftGeneratorWing_A_01",
    Player.pos + 5*Vector3f::UP
);

// set position/rotation
honse1.setPos(123, 456.0, 789.5);
honse1.pos += Vector3f::SOUTH * 6.9;
Player.pos += Vector3f(0, 100, 0);
Player.rot = Matrix33f::NORTHEAST;
PlayerCamera.setRot(1,0,0, 0,1,0, 0,0,1);
EventCamera.setPosRot(
    123, 456.0, 789.5, // xyz (engine)
    1,0,0, 0,1,0, 0,0,1 // Matrix33f
);

// watch an ActorBase by absolute address
ptr_t somewhere = 0x3331337; // (pointers are fake)
ActorWatcher::assignSlot(1, somewhere);
// watch a PreActor, follow its created ActorBase
ActorWatcher::assignSlotPreActor(1, somewhere);
ActorWatcher::clearSlot(1);

// next recall hover event
ActorWatcher::assignSlotAwaitRecall(1);

// next spawn (anything)
ActorWatcher::assignSlotAwaitSpawn(1, "");

// next named spawn (exact match)
ActorWatcher::assignSlotAwaitSpawn(1, "PouchPlayer");

// next actor spawned with banc entity hash
ActorWatcher::assignSlotAwaitBancEntityHash(1, 0x07b4e71cd2536055);

// ActorWatcher config
ActorWatcher::doTextWriter(0, false);
ActorWatcher::doWsLog(0, false);

ActorWatcher::doDrawAABB(0, true);
ActorWatcher::doDrawPos(0, false);
ActorWatcher::doDrawVel(0, false);
ActorWatcher::doDrawAngVel(0, false);

ActorWatcher::doDrawModelPos(0, false);

ActorWatcher::doDrawRigidBodyAABB(0, 0x8000000000000000); // main only
ActorWatcher::doDrawRigidBodyPos(0, 0xffffffffffffffff); // all
ActorWatcher::doDrawRigidBodyPosPast(0, 0); // none
ActorWatcher::doDrawRigidBodyPosFuture(0, 4+8); // flags -3 | -4

// access ActorBase name, aabb, ...
BoundBox3f aabb = Player.getAABB();
BoundBox3f wrld = Player.getBoundingBoxWorld();
TextWriter::toast(30, format(
    "{}: {} {} \n",
    Player.getName(), aabb.min_y, aabb.max_y
));

// set RigidBodys in motion
RigidBody@ rbody = ActorWatcher::get(1).getMainRigidBody();
rbody.setVel(0, 20.0, 0); // doesnt work on Player
rbody.vel += Vector3f(0, 20.0, 0);
rbody.applyImpulse(0, 5000.0, 0); // larger values, also no Player
rbody.applyImpulse(Vector3f(0, 5000.0, 0));

// access RigidBody name, aabb, lastTransform, ...
auto rbody = Player.getMainRigidBody();
auto aabb = rbody.getAABB();
TextWriter::toast(30, format(
    "{}: {} {} : {} \n",
    rbody.getName(), aabb.min_y, aabb.max_y, rbody.lastTransform.pos.y,
));

