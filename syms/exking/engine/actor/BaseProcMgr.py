from build_types import *
begin_sym_file(__file__)


"""
#include <cstdint>
typedef uint32_t u32;
namespace engine::actor {
    class ActorBase{};
    class BaseProcMgr { public:
                                           // XXX prob baseprocs
        u32 addDependency(BaseProcMgr&,    ActorBase&, ActorBase&) {}
        u32 removeDependency(BaseProcMgr&, ActorBase&, ActorBase&) {}
};}
engine::actor::BaseProcMgr x = {};
engine::actor::ActorBase y = {};
auto _1 = x.addDependency(x, y, y);
auto _2 = x.removeDependency(x, y, y);
"""

EXKING("_ZN6engine5actor11BaseProcMgr13addDependencyERS1_RNS0_9ActorBaseES4_", {
    TOTK_100: 0x023c72a0,
    TOTK_110: 0x00dc9360,
    TOTK_111: 0x00dc13f0,
    TOTK_112: 0x00d9fc2c,
    TOTK_120: 0x00dbe8c8,
    TOTK_121: 0x00d8b0ec,
})

EXKING("_ZN6engine5actor11BaseProcMgr16removeDependencyERS1_RNS0_9ActorBaseES4_", {
    TOTK_100: 0x00ab8610,
    TOTK_110: 0x008a6c80,
    TOTK_111: 0x008082d8,
    TOTK_112: 0x008b6e90,
    TOTK_120: 0x008b8b8c,
    TOTK_121: 0x008eee60,
})

# namespace engine::actor::BaseProcMgr { static uintptr_t sInstance; }
EXKING("_ZN6engine5actor11BaseProcMgrL9sInstanceE", {
    TOTK_100: 0x04647330,
    TOTK_110: 0x04724988,
    TOTK_121: 0x04722268,
}, SymbolType.DATA)
