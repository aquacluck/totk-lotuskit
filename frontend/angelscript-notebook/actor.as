// spawned near Player by default
actor::createSimple("GameRomHorse");

// spawn at coordinates (engine coordinates, not display)
actor::createSimple("GameRomHorse", 123, 456.0, 789.5);

// assign to ActorWatcher slot 1 on creation
actor::createAndWatch(1, "GameRomHorse");

// wait for actor to spawn before accessing
// (see [tas.as] for more info on timing)
tas::input(8, NONE, 0,0, 0,0);

// get a handle to the watched horse
ActorBase@ honse1 = ActorWatcher::get(1);

// some actors like Player have permanent handles
ActorWatcher::assignSlot(0, Player); // default slot
ActorWatcher::assignSlot(0, honse1);

// actor position can be read and written with pos_*
if (Player.pos_y < honse1.pos_y) {
    Player.pos_y = honse1.pos_y + 5;
}

// spawn a wing below Player, watch it in slot 2
actor::createAndWatch(
    2, "SpObj_LiftGeneratorWing_A_01",
    Player.pos_x, Player.pos_y - 5, Player.pos_z
);

// set position/rotation all at once
honse1.setPos(123, 456.0, 789.5);
PlayerCamera.setRot(1,0,0, 0,1,0, 0,0,1);
EventCamera.setPosRot(
    123, 456.0, 789.5, // xyz (engine)
    1,0,0, 0,1,0, 0,0,1 // Matrix33f
);

ptr_t somewhere = 0x3331337; // pointers are fake
// watch an ActorBase by absolute address
ActorWatcher::assignSlot(1, somewhere);
// watch a PreActor, follow its created ActorBase
ActorWatcher::assignSlotPreActor(1, somewhere);
ActorWatcher::clearSlot(1);

