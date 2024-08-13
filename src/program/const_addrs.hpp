#pragma once
// XXX ptrdiff_t?

#ifdef TOTK_100
static const int s_WorldManagerModule_BaseProc = 0x00b22a34;
static const int s_ActorMgr_addActorRelation = 0x023c72a0;
static const int s_ActorMgr_resolveActorRelation = 0x00ab8610;
static const int s_VFRMgr_sInstance = 0x04661598;
static const int s_nnMain_preMainLoop = 0x00e998b8; // strb w8 ,[x21 , #0x8 ]

#elif TOTK_110
static const int s_WorldManagerModule_BaseProc = 0x00af225c;
static const int s_ActorMgr_addActorRelation = 0x00dc9360;
static const int s_ActorMgr_resolveActorRelation = 0x008a6c80;
static const int s_VFRMgr_sInstance = nullptr; // FIXME
static const int s_nnMain_preMainLoop = nullptr; // FIXME

#elif TOTK_121
static const int s_WorldManagerModule_BaseProc = 0x00b36920;
static const int s_ActorMgr_addActorRelation = 0x00d8b0ec;
static const int s_ActorMgr_resolveActorRelation = 0x008eee60;
static const int s_VFRMgr_sInstance = 0x04725bb8;
static const int s_nnMain_preMainLoop = 0x00e7ac48; // strb w8 ,[x21 , #0x8 ]. blr at 7100e7ac60 is point of no return? not sure what that call is


static const int s_xxx_createActor1 = 0x0117f4a4; // createActorAndConnectSync
static const int s_xxx_createActor2 = 0x0140cb58; // createActorAndConnectSync(?)
static const int s_xxx_createActor3 = 0x012b13dc; // createActorAndConnectSyncImpl
static const int s_xxx_createActor4 = 0x00ab92cc; // requestCreateActorAsync
static const int s_xxx_createActor5 = 0x012a8b44; // requestCreateActorSync

#endif


#ifdef TOTK_100
static const int s_tryGetPlayerPhysicsPosPtrHook = 0x0022ed04;
#else
static const int s_tryGetPlayerPhysicsPosPtrHook = 0x0020d98c;
#endif


/*
//#elif TOTK_111
//#elif TOTK_112
//#elif TOTK_120

// these first three are from another thing I was trying and aren't used in this specific mod
// this first one hooks the WorldManagerModule's baseproc to print Link's current WorldMgr position
static constexpr int s_WorldManagerModule_BaseProc[] = {
    0x00b22a34,
    0x00af225c,
    0x00b56d34,
    0x00ae0848,
    0x00aef4b4,
    0x00b36920
};

// this one hooks AutoSaveMgr's calc function to print Link's current save position
static constexpr int s_calc[] = {
    0x00c2e288,
    0x00c2f074,
    0x00c19d90,
    0x00c26e8c,
    0x00b76848,
    0x00c2cbd0
};

// this one hooks the PlayerRef component calc function to print the Player actor's position
static constexpr int s_poshook[] = {
    0x009632b8,
    0x007eb780,
    0x007393dc,
    0x0081b924,
    0x00950e28,
    0x009e558c
};

// hooks engine::actor::ActorMgr::addActorRelation
static constexpr int s_ActorMgr_addActorRelation[] = {
    0x023c72a0,
    0x00dc9360,
    0x00dc13f0,
    0x00d9fc2c,
    0x00dbe8c8,
    0x00d8b0ec
};

// hooks engine::actor::ActorMgr::resolveActorRelation
static constexpr int s_ActorMgr_resolveActorRelation[] = {
    0x00ab8610,
    0x008a6c80,
    0x008082d8,
    0x008b6e90,
    0x008b8b8c,
    0x008eee60
};

static constexpr int s_getPlayerActorLink[] = {
    0x00bc2b38,
    0x00000000, // TODO other versions
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000
};
*/
