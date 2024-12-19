from build_types import *
begin_sym_file(__file__)

'''
namespace engine::actor::ActorBase {
    void setPositionAndRotationDirect(void*) {} // wrong sig, just 4 hook
    void updatePositionAndRotation(void*) {} // wrong sig, just 4 hook
    void forceSetMatrix(void*) {} // wrong sig, just 4 hook. nasty arg so i wanna wrap it anyways
}
'''

EXKING("_ZN6engine5actor9ActorBase28setPositionAndRotationDirectEPv", {
    TOTK_100: 0x007f13c0,
    TOTK_110: 0x00a2e240,
    TOTK_121: 0x00860020,
})

EXKING("_ZN6engine5actor9ActorBase25updatePositionAndRotationEPv", {
    TOTK_100: 0x007b76b8,
    TOTK_110: 0x009f1924,
    TOTK_121: 0x00823fd8,
})

EXKING("_ZN6engine5actor9ActorBase14forceSetMatrixEPv", {
    TOTK_100: 0x0069a18c,
    TOTK_110: 0x0070e204,
    TOTK_121: 0x006ba86c,
})

