from build_types import *
begin_sym_file(__file__)

# namespace engine { static uintptr_t nnMain_post_setup; }
EXKING("_ZN6engineL17nnMain_post_setupE", {
    TOTK_100: 0x00e998b8,
    TOTK_110: 0x00eb8f08,
    TOTK_121: 0x00e7ac48, # strb w8 ,[x21 , #0x8 ]. blr at 7100e7ac60 is point of no return? not sure what that call is
}, SymbolType.INSTRUCTION)
