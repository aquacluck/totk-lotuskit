#pragma once
#include <cstddef>



/* ======== BEGIN nnrtld (nso rtld, symbol __rtld_start) =========== */


/* ======== END nnrtld (nso rtld) ================================ */



/* ======== BEGIN EX-King (nso main, symbol __main_start) =========== */


namespace sym::agl::lyr::RenderDisplay::drawLayer_ {
class ensure_font {
    public:
    inline static constexpr ptrdiff_t offset = 0x00818340;
};
}

namespace sym::agl::lyr::Layer {
class drawDebugInfo_ {
    public:
    inline static constexpr ptrdiff_t offset = 0x0081911c;
};
}

namespace sym::agl::lyr::Renderer {
class draw {
    public:
    inline static constexpr ptrdiff_t offset = 0x00bc81ac;
};
}

namespace sym::agl {
class create_arg {
    public:
    inline static constexpr ptrdiff_t offset = 0x00a9123c;
};
}

namespace sym::agl {
class default_font {
    public:
    inline static constexpr ptrdiff_t offset = 0x04716af8;
};
}

namespace sym::agl {
class init_debug_drawers {
    public:
    inline static constexpr ptrdiff_t offset = 0x00a92964;
};
}

namespace sym::game::ai::execute::ExecutePlayerWhistle {
class enterImpl_ {
    public:
    inline static constexpr ptrdiff_t offset = 0x01d8feb4;
};
}

namespace sym::game::ai::execute::ExecutePlayerReverseRecorder::updateImpl_ {
class state0 {
    public:
    inline static constexpr ptrdiff_t offset = 0x01d76ef4;
};
}

namespace sym::game::wm::WorldManagerModule {
class baseProcExe {
    public:
    inline static constexpr ptrdiff_t offset = 0x00b36920;
};
}

namespace sym::sead::TextWriter {
class setupGraphics {
    public:
    inline static constexpr ptrdiff_t offset = 0x01888e6c;
};
}

namespace sym::sead::TextWriter {
class setCursorFromTopLeft {
    public:
    inline static constexpr ptrdiff_t offset = 0x010ad4cc;
};
}

namespace sym::sead::TextWriter {
class printf {
    public:
    inline static constexpr ptrdiff_t offset = 0x018890d4;
};
}

namespace sym::sead::TextWriter {
class ctor {
    public:
    inline static constexpr ptrdiff_t offset = 0x010ad510;
};
}

namespace sym::sead::TextWriter {
class dtor {
    public:
    inline static constexpr ptrdiff_t offset = 0x010ad478;
};
}

namespace sym::sead::GlobalRandom {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04716f08;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawDisk32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c3a7c;
};
}

namespace sym::sead::PrimitiveDrawer {
class setDrawCtx {
    public:
    inline static constexpr ptrdiff_t offset = 0x022707c0;
};
}

namespace sym::sead::PrimitiveDrawer {
class setModelMtx {
    public:
    inline static constexpr ptrdiff_t offset = 0x02270118;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawCircle32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c3b20;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawWireCube {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c372c;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawSphere4x8 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c393c;
};
}

namespace sym::sead::PrimitiveDrawer {
class setProjection {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c362c;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawCylinder16 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c3bac;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawCylinder32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c3c48;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawSphere8x16 {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c39d8;
};
}

namespace sym::sead::PrimitiveDrawer {
class end {
    public:
    inline static constexpr ptrdiff_t offset = 0x022707f0;
};
}

namespace sym::sead::PrimitiveDrawer {
class begin {
    public:
    inline static constexpr ptrdiff_t offset = 0x022707c8;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawBox {
    public:
    inline static constexpr ptrdiff_t offset = 0x02270904;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawLine {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c37bc;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawQuad {
    public:
    inline static constexpr ptrdiff_t offset = 0x021c3654;
};
}

namespace sym::sead::PrimitiveDrawer {
class drawQuad2 {
    public:
    inline static constexpr ptrdiff_t offset = 0x022708d8;
};
}

namespace sym::sead::PrimitiveDrawer {
class setCamera {
    public:
    inline static constexpr ptrdiff_t offset = 0x0227010c;
};
}

namespace sym::sead::PrimitiveRenderer {
class instance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04718c80;
};
}

namespace sym::sead::Random {
class getU32 {
    public:
    inline static constexpr ptrdiff_t offset = 0x010f1c6c;
};
}

namespace sym::sead::HeapMgr {
class sRootHeaps {
    public:
    inline static constexpr ptrdiff_t offset = 0x04716c08;
};
}

namespace sym::sead::HeapMgr {
class sInstancePtr {
    public:
    inline static constexpr ptrdiff_t offset = 0x04716ca0;
};
}

namespace sym::sead::HeapMgr {
class createRootHeap_ {
    public:
    inline static constexpr ptrdiff_t offset = 0x00e7cdc4;
};
}

namespace sym::sead::FrameHeap {
class create {
    public:
    inline static constexpr ptrdiff_t offset = 0x00ac2bfc;
};
}

namespace sym::sead::ThreadMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04716e00;
};
}

namespace sym::phive::RigidBodyBase {
class getAABB {
    public:
    inline static constexpr ptrdiff_t offset = 0x00b7d5d4;
};
}

namespace sym::phive::RigidBodyBase {
class getName {
    public:
    inline static constexpr ptrdiff_t offset = 0x02a911b8;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class requestCreateActorSync {
    public:
    inline static constexpr ptrdiff_t offset = 0x012a8b44;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class requestCreateActorAsync {
    public:
    inline static constexpr ptrdiff_t offset = 0x00ab92cc;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class createActorAndConnectSync {
    public:
    inline static constexpr ptrdiff_t offset = 0x0117f4a4;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class createActorAndConnectSync__2 {
    public:
    inline static constexpr ptrdiff_t offset = 0x0140cb58;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class createActorAndConnectSyncImpl {
    public:
    inline static constexpr ptrdiff_t offset = 0x012b13dc;
};
}

namespace sym::engine::actor::ActorInstanceMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04722920;
};
}

namespace sym::engine::actor::BaseProcCreateAndDeleteThread {
class calc_create {
    public:
    inline static constexpr ptrdiff_t offset = 0x0244a80c;
};
}

namespace sym::engine::actor::BaseProcCreateAndDeleteThread {
class calc_delete {
    public:
    inline static constexpr ptrdiff_t offset = 0x0244ac40;
};
}

namespace sym::engine::actor::ActorMgr {
class removeActorRelation {
    public:
    inline static constexpr ptrdiff_t offset = 0x008eee60;
};
}

namespace sym::engine::actor::ActorMgr {
class registerActorRelation {
    public:
    inline static constexpr ptrdiff_t offset = 0x00d8b0ec;
};
}

namespace sym::engine::actor::ActorMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04722268;
};
}

namespace sym::engine::actor::ActorBase {
class forceSetMatrix {
    public:
    inline static constexpr ptrdiff_t offset = 0x006ba86c;
};
}

namespace sym::engine::actor::ActorBase {
class updatePositionAndRotation {
    public:
    inline static constexpr ptrdiff_t offset = 0x00823fd8;
};
}

namespace sym::engine::actor::ActorBase {
class setPositionAndRotationDirect {
    public:
    inline static constexpr ptrdiff_t offset = 0x00860020;
};
}

namespace sym::engine::module::VFRMgr {
class sInstance {
    public:
    inline static constexpr ptrdiff_t offset = 0x04725bb8;
};
}

namespace sym::engine {
class steal_heap {
    public:
    inline static constexpr ptrdiff_t offset = 0x007f61d0;
};
}

namespace sym::engine {
class MainGetNpadStates {
    public:
    inline static constexpr ptrdiff_t offset = 0x02a267bc;
};
}

namespace sym::engine {
class nnMain_post_setup {
    public:
    inline static constexpr ptrdiff_t offset = 0x00e7ac48;
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
    inline static constexpr ptrdiff_t offset = 0x007fe124;
};
}

/* ======== END EX-King (nso main) ================================ */



/* ======== BEGIN multimedia (nso subsdk0, symbol __subsdk0_start) =========== */


/* ======== END multimedia (nso subsdk0) ================================ */



/* ======== BEGIN nnSdk (nso sdk, symbol __sdk_start) =========== */


/* ======== END nnSdk (nso sdk) ================================ */

