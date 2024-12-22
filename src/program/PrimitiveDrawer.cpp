#include "lib.hpp"
#include "syms_merged.hpp"
#include "PrimitiveDrawer.hpp"
#include "TextWriter.hpp"

#include <common/aglDrawContext.h>
#include <nn/util.h>

namespace lotuskit {
    namespace PrimitiveImpl {

        PrimitiveDrawerDrawNode* appendNewDrawNode(size_t drawList_i) {
            // alloc
            DebugDrawFrame* frame = lotuskit::TextWriter::currentDrawFrame;
            PrimitiveDrawerDrawNode* newNode = (PrimitiveDrawerDrawNode*)frame->heap->alloc(sizeof(PrimitiveDrawerDrawNode));
            newNode->primCallType = 0;
            newNode->primCallArgs = nullptr;
            newNode->next.store(nullptr);

            // append
            PrimitiveDrawerDrawNode* cmpNode = nullptr; // ensure null at time of write
            if (frame->drawLists3d[drawList_i].compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success -- appended to empty list
            // not null, enter the list
            PrimitiveDrawerDrawNode* node = frame->drawLists3d[drawList_i].load();
            while (true) {
                cmpNode = nullptr; // ensure null at time of write
                if (node->next.compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success
                // not null, traverse into next
                node = cmpNode;
            }

            return nullptr; // unreachable
        }

        template <typename T>
        T* allocNodeArgs(PrimitiveDrawerDrawNode* node) {
            DebugDrawFrame* frame = lotuskit::TextWriter::currentDrawFrame;
            auto ret = (T*)frame->heap->alloc(sizeof(T));
            node->primCallArgs = (void*)ret;
            return ret;
        }

        void drawFrame() {
            DebugDrawFrame* frame = lotuskit::TextWriter::currentDrawFrame;

            for (size_t i=0; i < DebugDrawFrame::MAX_DRAWLISTS; i++) {
                PrimitiveDrawerDrawNode* node = frame->drawLists3d[i].load();
                if (node == nullptr) { continue; }

                do {
                    if (node->primCallType != 0) {
                        PrimitiveImpl::dispatch(node->primCallType, node->primCallArgs);
                    }
                    node = node->next.load();

                } while (node != nullptr);

            }
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
                    auto n = (PrimitiveDrawerDrawNodeArgs::Quad*)node;
                    PrimitiveImpl::drawQuad(n->arg);
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
        void setModelMtx(size_t drawList_i, sead::Matrix34f mtx) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::SetModelMtx>(newNode);
            newNode->primCallType = 1;
            args->mtx = mtx;
        }
        void setProjection(size_t drawList_i, sead::Projection* projection) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::SetProjection>(newNode);
            newNode->primCallType = 2;
            args->projection = projection;
        }
        void setCamera(size_t drawList_i, sead::Camera* camera) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::SetCamera>(newNode);
            newNode->primCallType = 3;
            args->camera = camera;
        }
        void setDrawCtx(size_t drawList_i, sead::DrawContext* draw_ctx) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::SetDrawCtx>(newNode);
            newNode->primCallType = 4;
            args->draw_ctx = draw_ctx;
        }
        void drawQuad(size_t drawList_i, const sead::PrimitiveDrawer::QuadArg& arg) { // TODO de-overload these? cpp overloads in AS bindings can be painful
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Quad>(newNode);
            newNode->primCallType = 5;
            args->arg = arg;
        }
        void drawQuad(size_t drawList_i, const sead::Color4f& c0, const sead::Color4f& c1) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Quad2>(newNode);
            newNode->primCallType = 6;
            args->c0 = c0;
            args->c1 = c1;
        }
        void drawBox(size_t drawList_i, const sead::Color4f& c0, const sead::Color4f& c1) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Box>(newNode);
            newNode->primCallType = 7;
            args->c0 = c0;
            args->c1 = c1;
        }
        void drawWireCube(size_t drawList_i, const sead::PrimitiveDrawer::CubeArg& arg) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::WireCube>(newNode);
            newNode->primCallType = 8;
            args->arg = arg;
        }
        void drawLine(size_t drawList_i, const sead::Vector3f& start, const sead::Vector3f& end, const sead::Color4f& c0, const sead::Color4f& c1) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Line>(newNode);
            newNode->primCallType = 9;
            args->start = start;
            args->end = end;
            args->c0 = c0;
            args->c1 = c1;
        }
        void drawSphere4x8(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Sphere4x8>(newNode);
            newNode->primCallType = 10;
            args->pos = pos;
            args->radius = radius;
            args->north = north;
            args->south = south;
        }
        void drawSphere8x16(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& north, const sead::Color4f& south) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Sphere8x16>(newNode);
            newNode->primCallType = 11;
            args->pos = pos;
            args->radius = radius;
            args->north = north;
            args->south = south;
        }
        void drawDisk32(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& center, const sead::Color4f& edge) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Disk32>(newNode);
            newNode->primCallType = 12;
            args->pos = pos;
            args->radius = radius;
            args->center = center;
            args->edge = edge;
        }
        void drawCircle32(size_t drawList_i, const sead::Vector3f& pos, float radius, const sead::Color4f& color) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Circle32>(newNode);
            newNode->primCallType = 13;
            args->pos = pos;
            args->radius = radius;
            args->color = color;
        }
        void drawCylinder16(size_t drawList_i, const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Cylinder16>(newNode);
            newNode->primCallType = 14;
            args->pos = pos;
            args->radius = radius;
            args->height = height;
            args->top = top;
            args->btm = btm;
        }
        void drawCylinder32(size_t drawList_i, const sead::Vector3f& pos, float radius, float height, const sead::Color4f& top, const sead::Color4f& btm) {
            auto* newNode = PrimitiveImpl::appendNewDrawNode(drawList_i);
            auto* args = PrimitiveImpl::allocNodeArgs<PrimitiveDrawerDrawNodeArgs::Cylinder32>(newNode);
            newNode->primCallType = 15;
            args->pos = pos;
            args->radius = radius;
            args->height = height;
            args->top = top;
            args->btm = btm;
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
            renderer = exl::util::pointer_path::FollowSafe<sead::PrimitiveRenderer*, sym::sead::PrimitiveRenderer::instance::offset>();

            Begin = reinterpret_cast<BeginFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::begin::offset));
            End = reinterpret_cast<EndFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::end::offset));
            SetModelMtx = reinterpret_cast<SetModelMtxFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::setModelMtx::offset));
            SetProjection = reinterpret_cast<SetProjectionFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::setProjection::offset));
            SetCamera = reinterpret_cast<SetCameraFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::setCamera::offset));
            SetDrawCtx = reinterpret_cast<SetDrawCtxFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::setDrawCtx::offset));

            DrawQuad = reinterpret_cast<DrawQuadFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawQuad::offset));
            DrawQuad2 = reinterpret_cast<DrawQuad2Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawQuad2::offset));
            DrawBox = reinterpret_cast<DrawBoxFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawBox::offset));
            DrawWireCube = reinterpret_cast<DrawWireCubeFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawWireCube::offset));
            DrawLine = reinterpret_cast<DrawLineFunc*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawLine::offset));
            DrawSphere4x8 = reinterpret_cast<DrawSphere4x8Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawSphere4x8::offset));
            DrawSphere8x16 = reinterpret_cast<DrawSphere8x16Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawSphere8x16::offset));
            DrawDisk32 = reinterpret_cast<DrawDisk32Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawDisk32::offset));
            DrawCircle32 = reinterpret_cast<DrawCircle32Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawCircle32::offset));
            DrawCylinder16 = reinterpret_cast<DrawCylinder16Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawCylinder16::offset));
            DrawCylinder32 = reinterpret_cast<DrawCylinder32Func*>(exl::util::modules::GetTargetOffset(sym::sead::PrimitiveDrawer::drawCylinder32::offset));
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

        void drawQuad(const sead::PrimitiveDrawer::QuadArg& arg) {
            DrawQuad(getPrimitiveDrawer(), arg);
        }

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
