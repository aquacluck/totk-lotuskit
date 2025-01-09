from build_types import *
begin_sym_file(__file__)

EXKING("sead::PrimitiveRenderer::instance", {
    TOTK_100: 0x0463de50,
    TOTK_110: 0x0471b3a0,
    TOTK_121: 0x04718c80,
})

EXKING("sead::PrimitiveDrawer::begin", {
    TOTK_100: 0x021f74f8,
    TOTK_110: 0x022787f8,
    TOTK_121: 0x022707c8,
})

EXKING("sead::PrimitiveDrawer::end", {
    TOTK_100: 0x021f7520,
    TOTK_110: 0x02278820,
    TOTK_121: 0x022707f0,
})


# sead::PrimitiveDrawer::drawQuad(const sead::PrimitiveDrawer::QuadArg&)
EXKING("sead::PrimitiveDrawer::drawQuad", {
    TOTK_100: 0x0214ebf4,
    TOTK_110: 0x021cdd44,
    TOTK_121: 0x021c3654,
})

# sead::PrimitiveDrawer::drawQuad(const sead::Color4f&, const sead::Color4f&)
EXKING("sead::PrimitiveDrawer::drawQuad2", {
    TOTK_100: 0x021f7608,
    TOTK_110: 0x02278908,
    TOTK_121: 0x022708d8,
})

EXKING("sead::PrimitiveDrawer::drawBox", {
    TOTK_100: 0x021f7634,
    TOTK_110: 0x02278934,
    TOTK_121: 0x02270904,
})

EXKING("sead::PrimitiveDrawer::drawWireCube", {
    TOTK_100: 0x0214eccc,
    TOTK_110: 0x021cde1c,
    TOTK_121: 0x021c372c,
})

EXKING("sead::PrimitiveDrawer::drawLine", {
    TOTK_100: 0x0214ed5c,
    TOTK_110: 0x021cdeac,
    TOTK_121: 0x021c37bc,
})

EXKING("sead::PrimitiveDrawer::drawSphere4x8", {
    TOTK_100: 0x0214eedc,
    TOTK_110: 0x021ce02c,
    TOTK_121: 0x021c393c,
})

EXKING("sead::PrimitiveDrawer::drawSphere8x16", {
    TOTK_100: 0x0214ef78,
    TOTK_110: 0x021ce0c8,
    TOTK_121: 0x021c39d8,
})

EXKING("sead::PrimitiveDrawer::drawDisk32", {
    TOTK_100: 0x0214f01c,
    TOTK_110: 0x021ce16c,
    TOTK_121: 0x021c3a7c,
})

EXKING("sead::PrimitiveDrawer::drawCircle32", {
    TOTK_100: 0x0214f0c0,
    TOTK_110: 0x021ce210,
    TOTK_121: 0x021c3b20,
})

EXKING("sead::PrimitiveDrawer::drawCylinder16", {
    TOTK_100: 0x0214f14c,
    TOTK_110: 0x021ce29c,
    TOTK_121: 0x021c3bac,
})

EXKING("sead::PrimitiveDrawer::drawCylinder32", {
    TOTK_100: 0x0214f1e8,
    TOTK_110: 0x021ce338,
    TOTK_121: 0x021c3c48,
})

EXKING("sead::PrimitiveDrawer::setModelMtx", {
    TOTK_100: 0x021f6e48,
    TOTK_110: 0x02278148,
    TOTK_121: 0x02270118,
})

EXKING("sead::PrimitiveDrawer::setProjection", {
    TOTK_100: 0x0214ebcc,
    TOTK_110: 0x021cdd1c,
    TOTK_121: 0x021c362c,
})

EXKING("sead::PrimitiveDrawer::setCamera", {
    TOTK_100: 0x021f6e3c,
    TOTK_110: 0x0227813c,
    TOTK_121: 0x0227010c,
})

EXKING("sead::PrimitiveDrawer::setDrawCtx", {
    TOTK_100: 0x021f74f0,
    TOTK_110: 0x022787f0,
    TOTK_121: 0x022707c0,
})

# TODO move this elsewhere?
# static constexpr u64 sGetResidentActorMgrOffsets[6] = { 0, 0, 0, 0, 0, 0x00cdd728 })
# static constexpr u64 sFormalSceneOffsets[6] = { 0, 0, 0, 0, 0, 0x010d5024

