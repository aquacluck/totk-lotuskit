from build_types import *
begin_sym_file(__file__)

EXKING("engine::nnMain_post_setup", {
    TOTK_100: 0x00e998b8,
    TOTK_110: 0x00eb8f08,
    TOTK_121: 0x00e7ac48, # strb w8 ,[x21 , #0x8 ]. blr at 7100e7ac60 is point of no return? not sure what that call is
}, SymbolType.INSTRUCTION)

EXKING("engine::MainGetNpadStates", {
    TOTK_100: 0x029b442c,
    TOTK_110: 0x02a2c49c,
    TOTK_121: 0x02a267bc,
})

EXKING("engine::steal_heap", {
    TOTK_100: 0x008111a8,
    TOTK_110: 0x0083c2fc,
    TOTK_111: 0x00713b74,
    TOTK_112: 0x00802bfc,
    TOTK_120: 0x007a3428,
    TOTK_121: 0x007f61d0,
}, SymbolType.INSTRUCTION)

"""
# TODO register annotation/aliasing
MODULE("sym", {ALL: 0x0}, {
    "heap": {
        # ALL: "x22" ?
        TOTK_100: "x19",
        TOTK_121: "x22",
        # how to represent these? offset into inline_ctx might be handy but needs a human readable comment with it
        # how to expose to cpp? can we just stick them in another ns?
    }
}, SymbolType.INSTRUCTION)
"""

