from build_types import *
begin_sym_file(__file__)

# FIXME versions
# FIXME placeholder type sigs, hook only
'''
namespace engine::actor::ActorInstanceMgr {
    void* createActorAndConnectSync(void*) {}
    void* createActorAndConnectSync__2(void*) {}
    void* createActorAndConnectSyncImpl(void*) {}
    void* requestCreateActorAsync(void*) {}
    void* requestCreateActorSync(void*) {}
}
'''

EXKING("_ZN6engine5actor16ActorInstanceMgr25createActorAndConnectSyncEPv", {
    TOTK_100: 0x010c6838,
    TOTK_110: 0x01181d94,
    TOTK_121: 0x0117f4a4,
})
EXKING("_ZN6engine5actor16ActorInstanceMgr28createActorAndConnectSync__2EPv", {
    TOTK_100: 0x013e2d68,
    TOTK_110: 0x014204a4,
    TOTK_121: 0x0140cb58,
})
EXKING("_ZN6engine5actor16ActorInstanceMgr29createActorAndConnectSyncImplEPv", {
    TOTK_100: 0x01293710,
    TOTK_110: 0x012c3514,
    TOTK_121: 0x012b13dc,
})
EXKING("_ZN6engine5actor16ActorInstanceMgr23requestCreateActorAsyncEPv", {
    TOTK_100: 0x00a5fdf8,
    TOTK_110: 0x00bde0ac,
    TOTK_121: 0x00ab92cc,
})
EXKING("_ZN6engine5actor16ActorInstanceMgr22requestCreateActorSyncEPv", {
    TOTK_100: 0x01278724,
    TOTK_110: 0x012ba6c4,
    TOTK_121: 0x012a8b44,
})

# namespace engine::actor::ActorInstanceMgr { static uintptr_t sInstance; }
EXKING("_ZN6engine5actor16ActorInstanceMgrL9sInstanceE", {
    TOTK_100: 0x046479e0,
    TOTK_110: 0x04725040,
    TOTK_121: 0x04722920,
}, SymbolType.DATA)
