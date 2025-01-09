from build_types import *
begin_sym_file(__file__)

EXKING("game::component::GameCameraParam::HACK_cameraCalc", {
    TOTK_100: 0x00c7de84,
    TOTK_110: 0x00cdb894, # ty zeldar
    TOTK_121: 0x00cb4474,
})

# also sourced from zeldar, just keeping it here ig
EXKING("sead::LookAtCamera::doUpdateMatrix", {
    TOTK_100: 0x00a15d40,
    TOTK_110: 0x00a8b40c,
    TOTK_121: 0x00a2e4b0,
})
