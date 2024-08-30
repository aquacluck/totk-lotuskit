from build_types import *
begin_sym_file(__file__)

# TODO
# - basic mangle pass. if we actually call into the symbol (as opposed to hooking) it needs to perfectly match our callsites,
#   which will quickly become painful when passing typed pointers etc. Least we can do is make it easier to generate accurate symbol names.
# - Alternatively we could install a dummy hook at the symbol and call that instead of actually resolving the symbol, letting us define param types arbitrarily in the hook.
#   That seems too messy to do by default, but it might be preferable eg for certain overloaded calls.
# - Alternatively make a better exking header folder with proper namespacing, then generate symbols from headers:
#     - automatically build+parse an elf from the header -- this would have to be pretty crude
#     - foreach elf symbol demangle and see if it's desired
# - Alternatively parse used symbols out of subsdk9
# - Or for things we only plan to hook, adopt a placeholder format and drop correctness?

# namespace game::wm::WorldManagerModule { void baseProcExe(void*, void*) {} }
EXKING("_ZN4game2wm18WorldManagerModule11baseProcExeEPvS2_", {
    TOTK_100: 0x00b22a34,
    TOTK_110: 0x00af225c,
    TOTK_111: 0x00b56d34,
    TOTK_112: 0x00ae0848,
    TOTK_120: 0x00aef4b4,
    TOTK_121: 0x00b36920,
})

# sInstance
#TOTK_100: 71045f4ba8
#TOTK_110: 71046d06f8
#TOTK_121: 71046cdf78
