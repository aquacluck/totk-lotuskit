from build_types import *
begin_sym_file(__file__)

# FIXME real symbols? i'm always gonna wrap it all in one place anyways though...

# namespace sead::PrimitiveRenderer { static uintptr_t instance; }
EXKING("_ZN4sead17PrimitiveRendererL8instanceE", {
    TOTK_100: 0x0463de50,
    TOTK_110: 0x0471b3a0,
    TOTK_121: 0x04718c80,
})

EXKING("_ZN4sead15PrimitiveDrawerL5beginE", {
    TOTK_100: 0x021f74f8,
    TOTK_110: 0x022787f8,
    TOTK_121: 0x022707c8,
})

EXKING("_ZN4sead15PrimitiveDrawerL3endE", {
    TOTK_100: 0x021f7520,
    TOTK_110: 0x02278820,
    TOTK_121: 0x022707f0,
})


# sead::PrimitiveDrawer::drawQuad(const sead::PrimitiveDrawer::QuadArg&)
EXKING("_ZN4sead15PrimitiveDrawerL8drawQuadE", {
    TOTK_100: 0x0214ebf4,
    TOTK_110: 0x021cdd44,
    TOTK_121: 0x021c3654,
})

# sead::PrimitiveDrawer::drawQuad(const sead::Color4f&, const sead::Color4f&)
EXKING("_ZN4sead15PrimitiveDrawerL9drawQuad2E", {
    TOTK_100: 0x021f7608,
    TOTK_110: 0x02278908,
    TOTK_121: 0x022708d8,
})

EXKING("_ZN4sead15PrimitiveDrawerL7drawBoxE", {
    TOTK_100: 0x021f7634,
    TOTK_110: 0x02278934,
    TOTK_121: 0x02270904,
})

EXKING("_ZN4sead15PrimitiveDrawerL12drawWireCubeE", {
    TOTK_100: 0x0214eccc,
    TOTK_110: 0x021cde1c,
    TOTK_121: 0x021c372c,
})

EXKING("_ZN4sead15PrimitiveDrawerL8drawLineE", {
    TOTK_100: 0x0214ed5c,
    TOTK_110: 0x021cdeac,
    TOTK_121: 0x021c37bc,
})

EXKING("_ZN4sead15PrimitiveDrawerL13drawSphere4x8E", {
    TOTK_100: 0x0214eedc,
    TOTK_110: 0x021ce02c,
    TOTK_121: 0x021c393c,
})

EXKING("_ZN4sead15PrimitiveDrawerL14drawSphere8x16E", {
    TOTK_100: 0x0214ef78,
    TOTK_110: 0x021ce0c8,
    TOTK_121: 0x021c39d8,
})

EXKING("_ZN4sead15PrimitiveDrawerL10drawDisk32E", {
    TOTK_100: 0x0214f01c,
    TOTK_110: 0x021ce16c,
    TOTK_121: 0x021c3a7c,
})

EXKING("_ZN4sead15PrimitiveDrawerL12drawCircle32E", {
    TOTK_100: 0x0214f0c0,
    TOTK_110: 0x021ce210,
    TOTK_121: 0x021c3b20,
})

EXKING("_ZN4sead15PrimitiveDrawerL14drawCylinder16E", {
    TOTK_100: 0x0214f14c,
    TOTK_110: 0x021ce29c,
    TOTK_121: 0x021c3bac,
})

EXKING("_ZN4sead15PrimitiveDrawerL14drawCylinder32E", {
    TOTK_100: 0x0214f1e8,
    TOTK_110: 0x021ce338,
    TOTK_121: 0x021c3c48,
})

EXKING("_ZN4sead15PrimitiveDrawerL11setModelMtxE", {
    TOTK_100: 0x021f6e48,
    TOTK_110: 0x02278148,
    TOTK_121: 0x02270118,
})

EXKING("_ZN4sead15PrimitiveDrawerL13setProjectionE", {
    TOTK_100: 0x0214ebcc,
    TOTK_110: 0x021cdd1c,
    TOTK_121: 0x021c362c,
})

EXKING("_ZN4sead15PrimitiveDrawerL9setCameraE", {
    TOTK_100: 0x021f6e3c,
    TOTK_110: 0x0227813c,
    TOTK_121: 0x0227010c,
})

EXKING("_ZN4sead15PrimitiveDrawerL10setDrawCtxE", {
    TOTK_100: 0x021f74f0,
    TOTK_110: 0x022787f0,
    TOTK_121: 0x022707c0,
})

# TODO move this elsewhere?
# static constexpr u64 sGetResidentActorMgrOffsets[6] = { 0, 0, 0, 0, 0, 0x00cdd728 })
# static constexpr u64 sFormalSceneOffsets[6] = { 0, 0, 0, 0, 0, 0x010d5024

