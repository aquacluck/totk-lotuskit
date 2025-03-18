#include "exlaunch.hpp"
#include "PrimitiveDrawer.hpp"
#include "TextWriter.hpp"

#include <common/aglDrawContext.h>
#include <nn/util.h>

namespace lotuskit {
    namespace PrimitiveImpl {
        PrimitiveDrawerFrame frame = {0};

        void appendNewDrawNode(size_t drawList_i, u8 primCallType, size_t primCallArgsSize, void* primCallArgs) {
            if (frame.heap == nullptr) { return; } // uninitialized
            PrimitiveDrawerDrawNode* newNode = nullptr;
            PrimitiveDrawerDrawNode* cmpNode = nullptr;
            PrimitiveDrawerDrawNode* node = nullptr; // for traversal

            // drop concurrent + mid-draw requests
            if (!nn::os::TryLockMutex(&frame.drawListLock)) { return; } // lock or fail

            // alloc + store
            newNode = (PrimitiveDrawerDrawNode*)frame.heap->alloc(sizeof(PrimitiveDrawerDrawNode));
            if (newNode == nullptr) { goto RELEASE_AND_RETURN; } // heap full (this can happen anytime Main 3D is blocked from drawing, eg quickmenu)
            newNode->primCallType = primCallType;
            newNode->primCallArgs = frame.heap->alloc(primCallArgsSize);
            if (newNode->primCallArgs == nullptr) { goto RELEASE_AND_RETURN; }
            std::memcpy(newNode->primCallArgs, primCallArgs, primCallArgsSize);
            newNode->next.store(nullptr);

            // append
            cmpNode = nullptr; // ensure null at time of write
            if (frame.drawLists[drawList_i].compare_exchange_strong(cmpNode, newNode, std::memory_order_acq_rel)) { goto RELEASE_AND_RETURN; } // success -- appended to empty list
            // not null, enter the list
            node = frame.drawLists[drawList_i].load();
            while (node) {
                cmpNode = nullptr; // ensure null at time of write
                if (node->next.compare_exchange_strong(cmpNode, newNode, std::memory_order_acq_rel)) { break; } // success
                // not null, traverse into next
                node = cmpNode;
            }

            RELEASE_AND_RETURN:
            nn::os::UnlockMutex(&frame.drawListLock);
        }

        void drawFrame(agl::lyr::Layer* layer, const agl::lyr::RenderInfo& info) {
            nn::os::LockMutex(&frame.drawListLock);

            lotuskit::PrimitiveImpl::setupRenderer(info);
            lotuskit::PrimitiveImpl::begin();

            for (size_t i=0; i < PrimitiveDrawerFrame::MAX_DRAWLISTS; i++) {
                PrimitiveDrawerDrawNode* node = frame.drawLists[i].load();
                if (node == nullptr) { continue; }

                do {
                    if (node->primCallType != 0) {
                        PrimitiveImpl::dispatch(node->primCallType, node->primCallArgs);
                    }

                    node = node->next.load();
                } while (node != nullptr);

                frame.drawLists[i].store(nullptr); // wipe for next frame
            }

            //TODO PrimitiveImpl::drawToasts3d()
            lotuskit::PrimitiveImpl::end();

            frame.heap->freeAll();
            nn::os::UnlockMutex(&frame.drawListLock);
        }

        void dispatch(u8 primCallType, void* node) {
            switch(primCallType) {
                case 0: break; // nop, shouldn't happen
                case 1: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::SetModelMtx*)node;
                    PrimitiveImpl::setModelMtx(n->mtx);
                    break;
                } case 2: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::SetProjection*)node;
                    PrimitiveImpl::setProjection(n->projection);
                    break;
                } case 3: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::SetCamera*)node;
                    PrimitiveImpl::setCamera(n->camera);
                    break;
                } case 4: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::SetDrawCtx*)node;
                    PrimitiveImpl::setDrawCtx(n->draw_ctx);
                    break;
                } case 5: {
                    //auto n = (PrimitiveDrawerDrawNodeArgs::Quad*)node;
                    //PrimitiveImpl::drawQuad(n->arg);
                    break;
                } case 6: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Quad2*)node;
                    PrimitiveImpl::drawQuad(n->c0, n->c1);
                    break;
                } case 7: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Box*)node;
                    PrimitiveImpl::drawBox(n->c0, n->c1);
                    break;
                } case 8: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::WireCube*)node;
                    PrimitiveImpl::drawWireCube(n->arg);
                    break;
                } case 9: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Line*)node;
                    PrimitiveImpl::drawLine(n->start, n->end, n->c0, n->c1);
                    break;
                } case 10: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Sphere4x8*)node;
                    PrimitiveImpl::drawSphere4x8(n->pos, n->radius, n->north, n->south);
                    break;
                } case 11: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Sphere8x16*)node;
                    PrimitiveImpl::drawSphere8x16(n->pos, n->radius, n->north, n->south);
                    break;
                } case 12: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Disk32*)node;
                    PrimitiveImpl::drawDisk32(n->pos, n->radius, n->center, n->edge);
                    break;
                } case 13: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Circle32*)node;
                    PrimitiveImpl::drawCircle32(n->pos, n->radius, n->color);
                    break;
                } case 14: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Cylinder16*)node;
                    PrimitiveImpl::drawCylinder16(n->pos, n->radius, n->height, n->top, n->btm);
                    break;
                } case 15: {
                    auto n = (PrimitiveDrawerDrawNodeArgs::Cylinder32*)node;
                    PrimitiveImpl::drawCylinder32(n->pos, n->radius, n->height, n->top, n->btm);
                    break;
                }
                break; // err
            }
        }

    } // ns

    namespace PrimitiveDrawer {
        void setModelMtx(size_t drawList_i, const sead::Matrix34f &mtx) {
            auto args = PrimitiveDrawerDrawNodeArgs::SetModelMtx{.mtx = mtx};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 1, sizeof(args), &args);
        }
        void setProjection(size_t drawList_i, sead::Projection* projection) {
            auto args = PrimitiveDrawerDrawNodeArgs::SetProjection{.projection = projection};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 2, sizeof(args), &args);
        }
        void setCamera(size_t drawList_i, sead::Camera* camera) {
            auto args = PrimitiveDrawerDrawNodeArgs::SetCamera{.camera = camera};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 3, sizeof(args), &args);
        }
        void setDrawCtx(size_t drawList_i, sead::DrawContext* draw_ctx) {
            auto args = PrimitiveDrawerDrawNodeArgs::SetDrawCtx{.draw_ctx = draw_ctx};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 4, sizeof(args), &args);
        }
        /*
        void drawQuad(size_t drawList_i, const sead::PrimitiveDrawer::QuadArg& arg) { // TODO de-overload these? cpp overloads in AS bindings can be painful
            auto args = PrimitiveDrawerDrawNodeArgs::Quad{.arg = arg};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 5, sizeof(args), &args);
        }
        */
        void drawQuad(size_t drawList_i, const sead::Color4f& c0, const sead::Color4f& c1) {
            auto args = PrimitiveDrawerDrawNodeArgs::Quad2{.c0 = c0, .c1 = c1};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 6, sizeof(args), &args);
        }
        void drawBox(size_t drawList_i, const sead::Color4f& c0, const sead::Color4f& c1) {
            auto args = PrimitiveDrawerDrawNodeArgs::Box{.c0 = c0, .c1 = c1};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 7, sizeof(args), &args);
        }
        void drawWireCube(size_t drawList_i, const sead::PrimitiveDrawer::CubeArg& arg) {
            auto args = PrimitiveDrawerDrawNodeArgs::WireCube{.arg = arg};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 8, sizeof(args), &args);
        }
        void drawLine(size_t drawList_i, const sead::Vector3f& start, const sead::Vector3f& end, const sead::Color4f& c0, const sead::Color4f& c1) {
            auto args = PrimitiveDrawerDrawNodeArgs::Line{.start = start, .end = end, .c0 = c0, .c1 = c1};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 9, sizeof(args), &args);
        }
        void drawSphere4x8(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south) {
            auto args = PrimitiveDrawerDrawNodeArgs::Sphere4x8{.pos = pos, .radius = radius, .north = north, .south = south};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 10, sizeof(args), &args);
        }
        void drawSphere8x16(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south) {
            auto args = PrimitiveDrawerDrawNodeArgs::Sphere8x16{.pos = pos, .radius = radius, .north = north, .south = south};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 11, sizeof(args), &args);
        }
        void drawDisk32(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& center, const sead::Color4f& edge) {
            auto args = PrimitiveDrawerDrawNodeArgs::Disk32{.pos = pos, .radius = radius, .center = center, .edge = edge};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 12, sizeof(args), &args);
        }
        void drawCircle32(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& color) {
            auto args = PrimitiveDrawerDrawNodeArgs::Circle32{.pos = pos, .radius = radius, .color = color};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 13, sizeof(args), &args);
        }
        void drawCylinder16(size_t drawList_i, const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm) {
            auto args = PrimitiveDrawerDrawNodeArgs::Cylinder16{.pos = pos, .radius = radius, .height = height, .top = top, .btm = btm};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 14, sizeof(args), &args);
        }
        void drawCylinder32(size_t drawList_i, const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm) {
            auto args = PrimitiveDrawerDrawNodeArgs::Cylinder32{.pos = pos, .radius = radius, .height = height, .top = top, .btm = btm};
            PrimitiveImpl::appendNewDrawNode(drawList_i, 15, sizeof(args), &args);
        }

    } // ns

    namespace PrimitiveImpl {
        // PrimitiveRenderer's draw functions don't exist in totk, we only have PrimitiveDrawer bc of aal::DebugDrawer
        inline static sead::PrimitiveRenderer** renderer = nullptr;
        inline static sead::Matrix34f mMatrix{1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f};

        // missing Disk16 and Circle16, don't seem to exist - could implement them ourselves but the others don't even work yet so whatever
        using BeginFunc = void (sead::PrimitiveDrawer*);
        using EndFunc = void (sead::PrimitiveDrawer*);
        using SetModelMtxFunc = void (sead::PrimitiveDrawer*, sead::Matrix34f*);
        using SetProjectionFunc = void (sead::PrimitiveDrawer*, sead::Projection*);
        using SetCameraFunc = void (sead::PrimitiveDrawer*, sead::Camera*);
        using SetDrawCtxFunc = void (sead::PrimitiveDrawer*, sead::DrawContext*);

        using DrawQuadFunc = void (sead::PrimitiveDrawer*, const sead::PrimitiveDrawer::QuadArg&);
        using DrawQuad2Func = void (sead::PrimitiveDrawer*, const sead::Color4f&, const sead::Color4f&);
        using DrawBoxFunc = void (sead::PrimitiveDrawer*, const sead::Color4f&, const sead::Color4f&);
        using DrawWireCubeFunc = void (sead::PrimitiveDrawer*, const sead::PrimitiveDrawer::CubeArg&);
        using DrawLineFunc = void (sead::PrimitiveDrawer*, const sead::Vector3f&, const sead::Vector3f&, const sead::Color4f&, const sead::Color4f&);
        using DrawSphere4x8Func = void (sead::PrimitiveDrawer*, const sead::Vector3f&, float, const sead::Color4f&, const sead::Color4f&);
        using DrawSphere8x16Func = void (sead::PrimitiveDrawer*, const sead::Vector3f&, float, const sead::Color4f&, const sead::Color4f&);
        using DrawDisk32Func = void (sead::PrimitiveDrawer*, const sead::Vector3f&, float, const sead::Color4f&, const sead::Color4f&);
        using DrawCircle32Func = void (sead::PrimitiveDrawer*, const sead::Vector3f&, float, const sead::Color4f&);
        using DrawCylinder16Func = void (sead::PrimitiveDrawer*, const sead::Vector3f&, float, float, const sead::Color4f&, const sead::Color4f&);
        using DrawCylinder32Func = void (sead::PrimitiveDrawer*, const sead::Vector3f&, float, float, const sead::Color4f&, const sead::Color4f&);

        inline static BeginFunc* Begin = nullptr;
        inline static EndFunc* End = nullptr;
        inline static SetModelMtxFunc* SetModelMtx = nullptr;
        inline static SetProjectionFunc* SetProjection = nullptr;
        inline static SetCameraFunc* SetCamera = nullptr;
        inline static SetDrawCtxFunc* SetDrawCtx = nullptr;

        inline static DrawQuadFunc* DrawQuad = nullptr;
        inline static DrawQuad2Func* DrawQuad2 = nullptr;
        inline static DrawBoxFunc* DrawBox = nullptr;
        inline static DrawWireCubeFunc* DrawWireCube = nullptr;
        inline static DrawLineFunc* DrawLine = nullptr;
        inline static DrawSphere4x8Func* DrawSphere4x8 = nullptr;
        inline static DrawSphere8x16Func* DrawSphere8x16 = nullptr;
        inline static DrawDisk32Func* DrawDisk32 = nullptr;
        inline static DrawCircle32Func* DrawCircle32 = nullptr;
        inline static DrawCylinder16Func* DrawCylinder16 = nullptr;
        inline static DrawCylinder32Func* DrawCylinder32 = nullptr;

        void setupStatic() {
            renderer = EXL_SYM_RESOLVE<sead::PrimitiveRenderer**>("sead::PrimitiveRenderer::instance");

            Begin = EXL_SYM_RESOLVE<BeginFunc*>("sead::PrimitiveDrawer::begin");
            End = EXL_SYM_RESOLVE<EndFunc*>("sead::PrimitiveDrawer::end");
            SetModelMtx = EXL_SYM_RESOLVE<SetModelMtxFunc*>("sead::PrimitiveDrawer::setModelMtx");
            SetProjection = EXL_SYM_RESOLVE<SetProjectionFunc*>("sead::PrimitiveDrawer::setProjection");
            SetCamera = EXL_SYM_RESOLVE<SetCameraFunc*>("sead::PrimitiveDrawer::setCamera");
            SetDrawCtx = EXL_SYM_RESOLVE<SetDrawCtxFunc*>("sead::PrimitiveDrawer::setDrawCtx");

            DrawQuad = EXL_SYM_RESOLVE<DrawQuadFunc*>("sead::PrimitiveDrawer::drawQuad");
            DrawQuad2 = EXL_SYM_RESOLVE<DrawQuad2Func*>("sead::PrimitiveDrawer::drawQuad2");
            DrawBox = EXL_SYM_RESOLVE<DrawBoxFunc*>("sead::PrimitiveDrawer::drawBox");
            DrawWireCube = EXL_SYM_RESOLVE<DrawWireCubeFunc*>("sead::PrimitiveDrawer::drawWireCube");
            DrawLine = EXL_SYM_RESOLVE<DrawLineFunc*>("sead::PrimitiveDrawer::drawLine");
            DrawSphere4x8 = EXL_SYM_RESOLVE<DrawSphere4x8Func*>("sead::PrimitiveDrawer::drawSphere4x8");
            DrawSphere8x16 = EXL_SYM_RESOLVE<DrawSphere8x16Func*>("sead::PrimitiveDrawer::drawSphere8x16");
            DrawDisk32 = EXL_SYM_RESOLVE<DrawDisk32Func*>("sead::PrimitiveDrawer::drawDisk32");
            DrawCircle32 = EXL_SYM_RESOLVE<DrawCircle32Func*>("sead::PrimitiveDrawer::drawCircle32");
            DrawCylinder16 = EXL_SYM_RESOLVE<DrawCylinder16Func*>("sead::PrimitiveDrawer::drawCylinder16");
            DrawCylinder32 = EXL_SYM_RESOLVE<DrawCylinder32Func*>("sead::PrimitiveDrawer::drawCylinder32");
        }

        sead::PrimitiveRenderer* getPrimitiveRenderer() {
            return *renderer;
        }

        sead::PrimitiveDrawer* getPrimitiveDrawer() {
            return (*renderer)->getPrimitiveDrawer();
        }

        void setupRenderer(const agl::lyr::RenderInfo& info) {
            PrimitiveImpl::setProjection(info.projection);
            PrimitiveImpl::setCamera(info.camera);
            PrimitiveImpl::setModelMtx(mMatrix);
            PrimitiveImpl::setDrawCtx(dynamic_cast<sead::DrawContext*>(info.draw_ctx));
        }

        void begin() {
            Begin(getPrimitiveDrawer());
        }

        void end() {
            End(getPrimitiveDrawer());
        }

        void setModelMtx(sead::Matrix34f mtx) {
            mMatrix = mtx;
            SetModelMtx(getPrimitiveDrawer(), &mMatrix);
        }

        void setProjection(sead::Projection* projection) {
            SetProjection(getPrimitiveDrawer(), projection);
        }

        void setCamera(sead::Camera* camera) {
            SetCamera(getPrimitiveDrawer(), camera);
        }

        void setDrawCtx(sead::DrawContext* draw_ctx) {
            SetDrawCtx(getPrimitiveDrawer(), draw_ctx);
        }

        /*
        void drawQuad(const sead::PrimitiveDrawer::QuadArg& arg) {
            DrawQuad(getPrimitiveDrawer(), arg);
        }
        */

        void drawQuad(const sead::Color4f& c0, const sead::Color4f& c1) {
            DrawQuad2(getPrimitiveDrawer(), c0, c1);
        }

        void drawBox(const sead::Color4f& c0, const sead::Color4f& c1) {
            DrawBox(getPrimitiveDrawer(), c0, c1);
        }

        void drawWireCube(const sead::PrimitiveDrawer::CubeArg& arg) {
            DrawWireCube(getPrimitiveDrawer(), arg);
        }

        void drawLine(const sead::Vector3f& start, const sead::Vector3f& end, const sead::Color4f& c0, const sead::Color4f& c1) {
            DrawLine(getPrimitiveDrawer(), start, end, c0, c1);
        }

        void drawSphere4x8(const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south) {
            DrawSphere4x8(getPrimitiveDrawer(), pos, radius, north, south);
        }

        void drawSphere8x16(const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south) {
            DrawSphere8x16(getPrimitiveDrawer(), pos, radius, north, south);
        }

        void drawDisk32(const sead::Vector3f& pos, float radius, const sead::Color4f& center, const sead::Color4f& edge) {
            DrawDisk32(getPrimitiveDrawer(), pos, radius, center, edge);
        }

        void drawCircle32(const sead::Vector3f& pos, float radius, const sead::Color4f& color) {
            DrawCircle32(getPrimitiveDrawer(), pos, radius, color);
        }

        void drawCylinder16(const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm) {
            DrawCylinder16(getPrimitiveDrawer(), pos, radius, height, top, btm);
        }

        void drawCylinder32(const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm) {
            DrawCylinder32(getPrimitiveDrawer(), pos, radius, height, top, btm);
        }
    } // ns

} // ns
