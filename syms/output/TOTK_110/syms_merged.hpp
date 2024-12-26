#pragma once
#include <cstddef>



/* ======== BEGIN nnrtld (nso rtld, symbol __rtld_start) =========== */


/* ======== END nnrtld (nso rtld) ================================ */



/* ======== BEGIN EX-King (nso main, symbol __main_start) =========== */


namespace sym::agl::lyr::RenderDisplay::drawLayer_ {
class ensure_font {
    public:
    inline static constexpr ptrdiff_t offset = 0x0083f5b8;
};
}

namespace sym::agl::lyr::Layer {
class drawDebugInfo_ {
    public:
    inline static constexpr ptrdiff_t offset = 0x0084002c;
};
}

namespace sym::agl::lyr::Renderer {
class draw {
    public:
    inline static constexpr ptrdiff_t offset = 0x00be8138;
};
}

namespace sym::agl {
class create_arg {
    public:
    inline static constexpr ptrdiff_t offset = 0x00adb7cc;
};
}

namespace sym::agl {
class default_font {
    public:
    inline static constexpr ptrdiff_t offset = 0x04719618;
};
}

namespace sym::agl {
class init_debug_drawers {
    public:
    inline static constexpr ptrdiff_t offset = 0x00adcef4;
};
}

namespace sym::game::ai::execute::ExecutePlayerWhistle {
class enterImpl_ {
    public:
    inline static constexpr ptrdiff_t offset = 0x01d9d014;
};
}

namespace sym::game::ai::execute::ExecutePlayerReverseRecorder::updateImpl_ {
class state0 {
    public:
    inline static constexpr ptrdiff_t offset = 0x01d84040;
};
}

namespace sym::game::wm::WorldManagerModule {
class baseProcExe {
    public:
    inline static constexpr ptrdiff_t offset = 0x00af225c;
};
}

namespace sym::sead::TextWriter {
class setupGraphics {
    public:
    inline static constexpr ptrdiff_t offset = 0x0189917c;
};
}

namespace sym::sead::TextWriter {
class setCursorFromTopLeft {
    public:
    inline static constexpr ptrdiff_t offset = 0x010bfdb4;
};
}

namespace sym::sead::TextWriter {
class printf {
    public:
    inline static constexpr ptrdiff_t offset = 0x018993e4;
};
}

namespace sym::sead::TextWriter {
class ctor {
    public:
    inline static constexpr ptrdiff_t offset = 0x010bfdf8;
};
}

namespace sym::sead::TextWriter {
class dtor {
    public:
    inline static constexpr ptrdiff_t offset = 0x010bfd60;
};
}

namespace sym::sead::GlobalRandom {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04719628;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawDisk32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021ce16c;
};
}

namespace sym::sead::PrimitiveDrawer {
class setDrawCtx {
    public:
    inline static constexpr ptrdiff_t offset = 0x022787f0;
};
}

namespace sym::sead::PrimitiveDrawer {
class setModelMtx {
    public:
    inline static constexpr ptrdiff_t offset = 0x02278148;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawCircle32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021ce210;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawWireCube {
    public:
    inline static constexpr ptrdiff_t offset = 0x021cde1c;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawSphere4x8 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021ce02c;
};
}

namespace sym::sead::PrimitiveDrawer {
class setProjection {
    public:
    inline static constexpr ptrdiff_t offset = 0x021cdd1c;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawCylinder16 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021ce29c;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawCylinder32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021ce338;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawSphere8x16 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021ce0c8;
};
}

namespace sym::sead::PrimitiveDrawer {
class end {
    public:
    inline static constexpr ptrdiff_t offset = 0x02278820;
};
}

namespace sym::sead::PrimitiveDrawer {
class begin {
    public:
    inline static constexpr ptrdiff_t offset = 0x022787f8;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawBox {
    public:
    inline static constexpr ptrdiff_t offset = 0x02278934;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawLine {
    public:
    inline static constexpr ptrdiff_t offset = 0x021cdeac;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawQuad {
    public:
    inline static constexpr ptrdiff_t offset = 0x021cdd44;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawQuad2 {
    public:
    inline static constexpr ptrdiff_t offset = 0x02278908;
};
}

namespace sym::sead::PrimitiveDrawer {
class setCamera {
    public:
    inline static constexpr ptrdiff_t offset = 0x0227813c;
};
}

namespace sym::sead::PrimitiveRenderer {
class instance {
    public:
    inline static constexpr ptrdiff_t offset = 0x0471b3a0;
};
}

namespace sym::sead::Random {
class getU32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x011095ac;
};
}

namespace sym::sead::HeapMgr {
class sRootHeaps {
    public:
    inline static constexpr ptrdiff_t offset = 0x04719328;
};
}

namespace sym::sead::HeapMgr {
class sInstancePtr {
    public:
    inline static constexpr ptrdiff_t offset = 0x047193c0;
};
}

namespace sym::sead::HeapMgr {
class createRootHeap_ {
    public:
    inline static constexpr ptrdiff_t offset = 0x00ebb084;
};
}

namespace sym::sead::FrameHeap {
class create {
    public:
    inline static constexpr ptrdiff_t offset = 0x00b51918;
};
}

namespace sym::sead::ThreadMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04719520;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class requestCreateActorSync {
    public:
    inline static constexpr ptrdiff_t offset = 0x012ba6c4;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class requestCreateActorAsync {
    public:
    inline static constexpr ptrdiff_t offset = 0x00bde0ac;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class createActorAndConnectSync {
    public:
    inline static constexpr ptrdiff_t offset = 0x01181d94;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class createActorAndConnectSync__2 {
    public:
    inline static constexpr ptrdiff_t offset = 0x014204a4;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class createActorAndConnectSyncImpl {
    public:
    inline static constexpr ptrdiff_t offset = 0x012c3514;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04725040;
};
}

namespace sym::engine::actor::BaseProcCreateAndDeleteThread {
class calc_create {
    public:
    inline static constexpr ptrdiff_t offset = 0x02450b8c;
};
}

namespace sym::engine::actor::BaseProcCreateAndDeleteThread {
class calc_delete {
    public:
    inline static constexpr ptrdiff_t offset = 0x02450f88;
};
}

namespace sym::engine::actor::ActorMgr {
class removeActorRelation {
    public:
    inline static constexpr ptrdiff_t offset = 0x008a6c80;
};
}

namespace sym::engine::actor::ActorMgr {
class registerActorRelation {
    public:
    inline static constexpr ptrdiff_t offset = 0x00dc9360;
};
}

namespace sym::engine::actor::ActorMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04724988;
};
}

namespace sym::engine::actor::ActorBase {
class forceSetMatrix {
    public:
    inline static constexpr ptrdiff_t offset = 0x0070e204;
};
}

namespace sym::engine::actor::ActorBase {
class updatePositionAndRotation {
    public:
    inline static constexpr ptrdiff_t offset = 0x009f1924;
};
}

namespace sym::engine::actor::ActorBase {
class setPositionAndRotationDirect {
    public:
    inline static constexpr ptrdiff_t offset = 0x00a2e240;
};
}

namespace sym::engine::module::VFRMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x047282d8;
};
}

namespace sym::engine {
class steal_heap {
    public:
    inline static constexpr ptrdiff_t offset = 0x0083c2fc;
};
}

namespace sym::engine {
class MainGetNpadStates {
    public:
    inline static constexpr ptrdiff_t offset = 0x02a2c49c;
};
}

namespace sym::engine {
class nnMain_post_setup {
    public:
    inline static constexpr ptrdiff_t offset = 0x00eb8f08;
};
}

namespace sym::hkUNKNOWN {
class UNKNOWN_applyMotion {
    public:
    inline static constexpr ptrdiff_t offset = 0x0020d98c;
};
}

namespace sym::sead::HeapMgr {
class findContainHeap {
    public:
    inline static constexpr ptrdiff_t offset = 0x007612b0;
};
}

/* ======== END EX-King (nso main) ================================ */



/* ======== BEGIN multimedia (nso subsdk0, symbol __subsdk0_start) =========== */


/* ======== END multimedia (nso subsdk0) ================================ */



/* ======== BEGIN nnSdk (nso sdk, symbol __sdk_start) =========== */


/* ======== END nnSdk (nso sdk) ================================ */

