from build_types import *
begin_sym_file(__file__)

# namespace phive::RigidBodyBase { static uintptr_t getAABB; } XXX trampoline only
EXKING("_ZN5phive13RigidBodyBaseL7getAABBE", {
    TOTK_100: 0x007c1834,
    TOTK_110: 0x00c2c050,
    TOTK_121: 0x00b7d5d4,
})

EXKING("_ZN5phive13RigidBodyBaseL19getBoundingBoxWorldE", {
    TOTK_100: 0x009865d0,
    TOTK_110: 0x00c2c050,
    TOTK_121: 0x00a3c1c4,
})

EXKING("_ZN5phive13RigidBodyBaseL7getNameE", {
    TOTK_100: 0x02a1e9e4,
    TOTK_110: 0x02a9750c,
    TOTK_121: 0x02a911b8,
})

EXKING("_ZN5phive13RigidBodyBaseL24requestSetLinearVelocityE", {
    TOTK_100: 0x00661dc0,
    #TOTK_100: 0x00751574, # XXX this is angvel
    TOTK_110: 0x007a4cfc, # XXX idk
    TOTK_121: 0x00acc874,
})

EXKING("_ZN5phive13RigidBodyBaseL18applyLinearImpulseE", {
    TOTK_100: 0x00c2bd7c,
    TOTK_110: 0x00ccc234,
    TOTK_121: 0x00cf600c,
})

EXKING("_ZN5phive13RigidBodyBaseL21getNextLinearVelocityE", {
    TOTK_100: 0x011613d0,
    TOTK_110: 0x011bb3b8,
    TOTK_121: 0x011b44ac,
})

