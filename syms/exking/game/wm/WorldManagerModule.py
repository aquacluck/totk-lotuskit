from build_types import *
begin_sym_file(__file__)

EXKING("game::wm::WorldManagerModule::baseProcExe", {
    TOTK_100: 0x00b22a34,
    TOTK_110: 0x00af225c,
    TOTK_111: 0x00b56d34,
    TOTK_112: 0x00ae0848,
    TOTK_120: 0x00aef4b4,
    TOTK_121: 0x00b36920,
})

EXKING("game::wm::WorldManagerModule::sInstance", {
    TOTK_100: 0x045f4ba8,
    TOTK_110: 0x046d06f8,
    TOTK_121: 0x046cdf78,
}, SymbolType.DATA)

EXKING("game::wm::WorldManagerModule::setWeather", {
    TOTK_100: 0x01da88a4,
    TOTK_110: 0x01e18f38,
    TOTK_121: 0x01e0d604,
})

EXKING("game::wm::WorldManagerModule::setTime", {
    TOTK_100: 0x01da8834, # XXX theres 3 of these that only vary on flag
    TOTK_110: 0x01e18ec8,
    TOTK_121: 0x01e0d594,
})

EXKING("game::wm::WorldManagerModule::setFogDensity", {
    TOTK_100: 0x01edfaf8,
    TOTK_110: 0x01f5d164,
    TOTK_121: 0x01f51eb8,
})

# ty zeldar
EXKING("game::wm::WorldManagerModule::fogDisable_HACK", {
    TOTK_100: 0x00a9e0c0,
    TOTK_110: 0x0096a528,
    TOTK_121: 0x009adb48,
}, SymbolType.INSTRUCTION)

EXKING("game::wm::WorldManagerModule::setWind", {
    TOTK_100: 0x01da7e94,
    TOTK_110: 0x01e18528,
    TOTK_121: 0x01e0cbf4,
})

