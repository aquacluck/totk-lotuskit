from build_types import *
begin_sym_file(__file__)

'''
#include <cstdint>
namespace engine::actor::BaseProcCreateAndDeleteThread {
    static uintptr_t calc_create;
    static uintptr_t calc_delete;
}
'''

EXKING("_ZN6engine5actor29BaseProcCreateAndDeleteThreadL11calc_createE", {
    TOTK_100: 0x023c7e94,
    TOTK_110: 0x02450b8c,
    TOTK_121: 0x0244a80c,
}, SymbolType.INSTRUCTION)

EXKING("_ZN6engine5actor29BaseProcCreateAndDeleteThreadL11calc_deleteE", {
    TOTK_100: 0x023c8248,
    TOTK_110: 0x02450f88,
    TOTK_121: 0x0244ac40,
}, SymbolType.INSTRUCTION)
