#pragma once
#include <atomic>
#include <nn/nn.h>
#include <gfx/seadColor.h>
#include <gfx/seadPrimitiveRenderer.h>
#include <heap/seadFrameHeap.h>
#include <heap/seadHeap.h>
#include <lyr/aglLayer.h>
#include <lyr/aglRenderInfo.h>
#include <math/seadMatrix.h>
#include <math/seadVector.hpp>
#include <cstring>

namespace sead {
    class Camera;
    class DrawContext;
    class Projection;
} // ns

namespace lotuskit {
    struct PrimitiveDrawerDrawNode {
        u8 primCallType;
        void* primCallArgs; // PrimitiveDrawerDrawNodeArgs
        std::atomic<PrimitiveDrawerDrawNode*> next;
    };

    // AS+scheduled+etc wrappers: Append shape to draw queue
    namespace PrimitiveDrawer {
        void setModelMtx(size_t drawList_i, sead::Matrix34f mtx);
        void setProjection(size_t drawList_i, sead::Projection* projection);
        void setCamera(size_t drawList_i, sead::Camera* camera);
        void setDrawCtx(size_t drawList_i, sead::DrawContext* draw_ctx);
        void drawQuad(size_t drawList_i, const sead::PrimitiveDrawer::QuadArg& arg); // TODO de-overload these? cpp overloads in AS bindings can be painful
        void drawQuad(size_t drawList_i, const sead::Color4f& c0, const sead::Color4f& c1);
        void drawBox(size_t drawList_i, const sead::Color4f& c0, const sead::Color4f& c1);
        void drawWireCube(size_t drawList_i, const sead::PrimitiveDrawer::CubeArg& arg);
        void drawLine(size_t drawList_i, const sead::Vector3f& start, const sead::Vector3f& end, const sead::Color4f& c0, const sead::Color4f& c1);
        void drawSphere4x8(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south);
        void drawSphere8x16(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south);
        void drawDisk32(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& center, const sead::Color4f& edge);
        void drawCircle32(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& color);
        void drawCylinder16(size_t drawList_i, const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm);
        void drawCylinder32(size_t drawList_i, const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm);
    } // ns

    // direct draw-time bindings
    namespace PrimitiveImpl {
        void setupStatic();
        void setupRenderer(const agl::lyr::RenderInfo& info);
        sead::PrimitiveRenderer* getPrimitiveRenderer();
        sead::PrimitiveDrawer* getPrimitiveDrawer();

        void begin();
        void end();

        void setModelMtx(sead::Matrix34f mtx);
        void setProjection(sead::Projection* projection);
        void setCamera(sead::Camera* camera);
        void setDrawCtx(sead::DrawContext* draw_ctx);
        void drawQuad(const sead::PrimitiveDrawer::QuadArg& arg); // TODO de-overload these? cpp overloads in AS bindings can be painful
        void drawQuad(const sead::Color4f& c0, const sead::Color4f& c1);
        void drawBox(const sead::Color4f& c0, const sead::Color4f& c1);
        void drawWireCube(const sead::PrimitiveDrawer::CubeArg& arg);
        void drawLine(const sead::Vector3f& start, const sead::Vector3f& end, const sead::Color4f& c0, const sead::Color4f& c1);
        void drawSphere4x8(const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south);
        void drawSphere8x16(const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south);
        void drawDisk32(const sead::Vector3f& pos, float radius, const sead::Color4f& center, const sead::Color4f& edge);
        void drawCircle32(const sead::Vector3f& pos, float radius, const sead::Color4f& color);
        void drawCylinder16(const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm);
        void drawCylinder32(const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm);
    } // ns

    // arg holders for deferred calls
    namespace PrimitiveDrawerDrawNodeArgs {
        struct SetModelMtx { sead::Matrix34f mtx; };
        struct SetProjection { sead::Projection* projection; };
        struct SetCamera { sead::Camera* camera; };
        struct SetDrawCtx { sead::DrawContext* draw_ctx; };
        struct Quad { sead::PrimitiveDrawer::QuadArg arg; }; // TODO de-overload these? cpp overloads in AS bindings can be painful
        struct Quad2 { sead::Color4f c0; sead::Color4f c1; };
        struct Box { sead::Color4f c0; sead::Color4f c1; };
        struct WireCube { sead::PrimitiveDrawer::CubeArg arg; };
        struct Line { sead::Vector3f start; sead::Vector3f end; sead::Color4f c0; sead::Color4f c1; };
        struct Sphere4x8 { sead::Vector3f pos; float radius; sead::Color4f north; sead::Color4f south; };
        struct Sphere8x16 { sead::Vector3f pos; float radius; sead::Color4f north; sead::Color4f south; };
        struct Disk32 { sead::Vector3f pos; float radius; sead::Color4f center; sead::Color4f edge; };
        struct Circle32 { sead::Vector3f pos; float radius; sead::Color4f color; };
        struct Cylinder16 { sead::Vector3f pos; float radius; float height; sead::Color4f top; sead::Color4f btm; };
        struct Cylinder32 { sead::Vector3f pos; float radius; float height; sead::Color4f top; sead::Color4f btm; };
    } // ns

    struct PrimitiveDrawerFrame {
        static constexpr size_t MAX_DRAWLISTS = 2;
        inline static std::atomic<PrimitiveDrawerDrawNode*> drawLists[MAX_DRAWLISTS] = {0};
        inline static sead::FrameHeap* heap = nullptr; // draw state is wiped each frame
        nn::os::MutexType drawLock;
    };

    /*
    struct PrimitiveRendererToastNode {
        u8 primCallType;
        void* primCallArgs;
        u64 ttlFrames;
    };
    */

    // drawlist storage -- TODO name PrimitiveStatic to separate ^ ?
    namespace PrimitiveImpl {
        extern PrimitiveDrawerFrame frame;
        inline static sead::Heap* debugDrawerInternalHeap = nullptr;

        //static constexpr size_t MAX_TOASTS = 0x20;
        //inline static std::atomic<PrimitiveRendererToastNode*> toasts3d[MAX_TOASTS] = {0};

        inline static void assignHeap(sead::Heap* heap) {
            // assert only called once
            debugDrawerInternalHeap = heap;
            frame.heap = sead::FrameHeap::create(0x4000, "lotuskit::PrimitiveDrawer", heap, 8, sead::Heap::cHeapDirection_Forward, 0); // XXX what are args 4+6?
            nn::os::InitializeMutex(&frame.drawLock, true, 0);
        }

        PrimitiveDrawerDrawNode* appendNewDrawNode(size_t drawList_i);
        void drawFrame(agl::lyr::Layer* layer, const agl::lyr::RenderInfo& info);
        void dispatch(u8 primCallType, void* node);
    } // ns

} // ns
