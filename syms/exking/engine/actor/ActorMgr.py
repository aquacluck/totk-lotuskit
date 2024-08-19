from build_types import *
begin_sym_file(__file__)


"""
#include <cstdint>
typedef uint32_t u32;
namespace engine::actor {
    class ActorBase{};
    class ActorMgr { public:
        u32 registerActorRelation(ActorMgr&, ActorBase&, ActorBase&) {}
        u32 removeActorRelation(ActorMgr&, ActorBase&, ActorBase&) {}
};}
engine::actor::ActorMgr x = {};
engine::actor::ActorBase y = {};
auto _1 = x.registerActorRelation(x, y, y);
auto _2 = x.removeActorRelation(x, y, y);
"""

EXKING("_ZN6engine5actor8ActorMgr21registerActorRelationERS1_RNS0_9ActorBaseES4_", {
    TOTK_100: 0x023c72a0,
    TOTK_110: 0x00dc9360,
    TOTK_111: 0x00dc13f0,
    TOTK_112: 0x00d9fc2c,
    TOTK_120: 0x00dbe8c8,
    TOTK_121: 0x00d8b0ec,
})

EXKING("_ZN6engine5actor8ActorMgr19removeActorRelationERS1_RNS0_9ActorBaseES4_", {
    TOTK_100: 0x00ab8610,
    TOTK_110: 0x008a6c80,
    TOTK_111: 0x008082d8,
    TOTK_112: 0x008b6e90,
    TOTK_120: 0x008b8b8c,
    TOTK_121: 0x008eee60,
})

