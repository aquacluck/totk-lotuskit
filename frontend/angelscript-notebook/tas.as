// do nothing for 0.5 seconds or 15 ideal frames
tas::input(30, NONE, 0,0, 0,0);

// tap dpad down and B button for 1 ideal frame,
// and hold rstick left just far enough to trigger input display
// (chosen because abs(x) > 16400 is when nnsdk flags it as a button press)
tas::input(2, KEY_DDOWN|KEY_B, 0,0, -16401,0);

// not "recording", just logging the inputs --
// for now saving+organizing them is entirely your job.
// (but i plan to add basic file support sometime)
tas::toggleDump();


/***********************************************************
/* ATTENTIONPLS: tas::input() takes 2 * desired frames
/*
/* For now, tas scheduling counts in 30fps half-frames,
/* which you can also think of as a fake 60fps
/* 1 frame @ 30fps = 2 @ 60fps
/*
/* VFRMgr will toggle between 20fps and 30fps effective speed,
/* so counting half-frames means we can still use whole numbers.
/* 3 frames @ 60fps = 1 @ 20fps
/*
/* This might be a bad idea and this might change, I'm no tas wizard,
/* but I don't want to mess around hardcoding framerates etc by default
/* when true-to-life conditions might be desirable for research+practice+etc
/**********************************************************/


/*
void tas::input(
     u32 duration60=2, u64 nextButtons=0,
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
    tas::input(2, KEY_PLUS, 0,0, 0,0); // pause
    tas::input(4, KEY_DLEFT, 0,0, 0,0); // wait + cursor left to target
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // swap to target
    tas::input(6, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(2, KEY_DDOWN, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // drop target
    tas::input(6, NONE, 0,0, 0,0);
    tas::input(2, KEY_PLUS, 0,0, 0,0); // unpause
    tas::input(6, NONE, 0,0, 0,0);
    tas::input(2, KEY_PLUS, 0,0, 0,0); // repause
    tas::input(2, NONE, 0,0, 0,0);
    // assert cursor is on equipped target
    // assert available filler to left of cursor
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(2, KEY_DDOWN, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // drop target
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(2, KEY_DLEFT, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(8, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // equip filler to left
    tas::input(6, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // unequip
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(2, KEY_PLUS, 0,0, 0,0); // unpause
    tas::input(6, NONE, 0,32767, 0,0); // up turnaround
    tas::input(2, KEY_PLUS, 0,0, 0,0); // repause
    tas::input(2, NONE, 0,0, 0,0);
    // assert cursor is on equipped target
    // assert available filler to left of cursor
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(2, KEY_DDOWN, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // drop target
    tas::input(2, NONE, 0,0, 0,0);
    tas::input(2, KEY_DLEFT, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(8, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // equip filler to left
    tas::input(6, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0);
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(2, KEY_A, 0,0, 0,0); // unequip
    tas::input(4, NONE, 0,0, 0,0);
    tas::input(2, KEY_PLUS, 0,0, 0,0); // unpause
    // manually turn around and take it, whatever
}


void example_cartslide1() {
    //actor::createSimple("Weapon_Sword_124"); // royal bsword
    // Weapon_Lsword_124 royal clay, SpObj_Cart_A_01, SpObj_Rocket_A_01
    //example_qds1(); // prereqs

    Player.setPosRot(-170, 437, -980, 1, 0, 0, 0, 1, 0, 0, 0, 1);
    tas::input(30, NONE); // TODO fix camera
    tas::input(30, KEY_ZL); // TODO fix camera

    // assert cart in quickmenu cursor, recall rune
    tas::input(12, KEY_DUP|KEY_ZL);
    tas::input(2, KEY_X|KEY_DUP|KEY_ZL);
    tas::input(38, KEY_ZL); // wait for drop

    // recall cart
    tas::input(2, KEY_L|KEY_ZL);
    tas::input(16, KEY_ZL, 0,0, 0,32767); //XXX inverted camera
    ActorWatcher::assignSlotAwaitRecall(1);
    tas::input(4, KEY_ZL); //XXX need to stop/slow cursor to select?
    tas::input(2, KEY_A|KEY_ZL); // begin recall
    tas::input(120, KEY_ZL); // wait for drop/origin position
    tas::input(2, KEY_X|KEY_ZL, 0,32767, 0,0); // jump on
    tas::input(60, KEY_ZL);
    tas::input(2, KEY_L|KEY_ZL); // end recall
}


// that is all :) ur doing great

