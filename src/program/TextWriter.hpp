#pragma once
#include "lib.hpp"
#include "syms_merged.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

#include "nn/nn.h"
#include "heap/seadHeap.h"
#include "math/seadVector.hpp"

// agl+gfx include
#include "common/aglDrawContext.h"
#include "lyr/aglLayer.h"
#include "lyr/aglRenderInfo.h"
#include "gfx/seadColor.h"
#include "gfx/seadDrawContext.h"
#include "gfx/seadTextWriter.h"


namespace lotuskit {
    class TextWriterExt: public sead::TextWriter {
        public:
        void getCursorFromTopLeftImpl(sead::Vector2f* pos) const {
            // always 720p sooooo just make it work
            pos->x = this->mCursor.x + 640.0;
            pos->y = 360.0 - this->mCursor.y;
        }
        void pprintf(sead::Vector2f &pos, const char* fmt, auto&&... args) {
            // pretty print: text shadow drawn first
            this->mColor.r = 0.0; this->mColor.g = 0.0; this->mColor.b = 0.0; this->mColor.a = 1.0; // black
            this->setCursorFromTopLeft(pos);
            this->printf(fmt, std::forward<decltype(args)>(args)...); // javascript lookin ass bullshit

            // main draw
            this->mColor.r = 0.85; this->mColor.g = 0.85; this->mColor.b = 0.85; this->mColor.a = 1.0; // white ish
            pos.x -= 1.0;
            pos.y -= 1.0;
            this->setCursorFromTopLeft(pos);
            this->printf(fmt, std::forward<decltype(args)>(args)...);

            // remember position for future calls
            this->getCursorFromTopLeftImpl(&pos);
            pos.x += 1.0;
            pos.y += 1.0;
        }
    };

    class TextWriter {
        public:
        inline static sead::Heap* debugDrawerHeap = nullptr;
        inline static void assignHeap(sead::Heap* heap) { debugDrawerHeap = heap; }
    };

    namespace TextWriterHooks {
        struct GraphicsModuleCreateArg {
            char _whatever[0xb4c];
            int value0;
            char _whatever2[0x10];
            int value1;
        };

        // used for DebugDrawEnsureFont
        HOOK_DEFINE_INLINE(GetCreateArg) {
            static const ptrdiff_t s_offset = sym::agl::create_arg::offset; // hacks
            static GraphicsModuleCreateArg create_arg;
            static void Callback(exl::hook::InlineCtx* ctx) {
                create_arg.value0 = reinterpret_cast<GraphicsModuleCreateArg*>(ctx->X[1])->value0;
                create_arg.value1 = reinterpret_cast<GraphicsModuleCreateArg*>(ctx->X[1])->value1; // nvnBufferBuilderSetStorage?
            }
        };

        HOOK_DEFINE_INLINE(DebugDrawEnsureFont) {
            static const ptrdiff_t s_offset = sym::agl::lyr::RenderDisplay::drawLayer_::ensure_font::offset; // hacks

            inline static nn::os::MutexType* fontMutex = nullptr;
            inline static void** sDefaultFont = nullptr;

            static void Setup(void) {
                fontMutex = new nn::os::MutexType();
                nn::os::InitializeMutex(fontMutex, true, 0);

                sDefaultFont = exl::util::pointer_path::FollowSafe<void*, sym::agl::default_font::offset>(); // hacks
            }

            static void Callback(exl::hook::InlineCtx* ctx) {
                // ignore other layers
                auto* layer = (agl::lyr::Layer*)(ctx->X[21]);
                if (strncmp("Tool 2D", layer->mLayerName.cstr(), 0x20) != 0) {
                    return;
                }

                // init font if needed
                if (*sDefaultFont == nullptr) {
                    nn::os::LockMutex(fontMutex);
                    if (*sDefaultFont == nullptr) {
                        Logger::logText("init default font", "/TextWriter");

                        void(*InitDebugDrawers)(sead::Heap*, GraphicsModuleCreateArg&) = nullptr;
                        void** tmp = (void**)(&InitDebugDrawers);
                        *tmp = exl::util::pointer_path::FollowSafe<void*, sym::agl::init_debug_drawers::offset>(); // hacks
                        InitDebugDrawers(TextWriter::debugDrawerHeap, GetCreateArg::create_arg);

                        if (*sDefaultFont == nullptr) {
                            Logger::logText("init default font fail", "/TextWriter");
                            return;
                        }
                    }
                    nn::os::UnlockMutex(fontMutex);
                }

                // allow layer to draw
                ctx->W[8] = 0x28; // satisfy check to call agl::lyr::Layer::drawDebugInfo_
                layer->mRenderFlags |= 1 << 13; // satisfy check inside vanilla drawDebugInfo_
            }
        };

        HOOK_DEFINE_TRAMPOLINE(DebugDrawHook) {
            static const ptrdiff_t s_offset = sym::agl::lyr::Layer::drawDebugInfo_::offset;
            static void Callback(agl::lyr::Layer* layer, const agl::lyr::RenderInfo& info) {
                // draw onto the given layer, always Tool 2D -- we would be given many layers if they weren't ignored above

                // XXX do we really need to do this every time? weird its a static. maybe we can hold onto one instead
                auto* sead_draw_ctx = dynamic_cast<sead::DrawContext*>(info.draw_ctx);
                sead::TextWriter::setupGraphics(sead_draw_ctx);
                sead::TextWriter _writer(sead_draw_ctx, info.viewport);
                TextWriterExt* writer = (TextWriterExt*)(&_writer);

                sead::Vector2f text_pos; // screen is always represented as 1280x720, upscaled for 1080p
                // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
                writer->mScale.x = 0.8;
                writer->mScale.y = 0.8;
                text_pos.x = 2.0;
                text_pos.y = 2.0;

                // no Player? either bootup or something very bad, unconditionally show feedback + version confirmation (if it'll even render) while we wait
                // TODO source+display high level configs (socket, svclog, etc)
                if (true) { //if (g_ModCommand_ActorWatcher[0].actor == nullptr) {
                    writer->pprintf(text_pos, "[totk-lotuskit:%d] awaiting Player, main_offset=%p\n", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
                }
            }
        };

    } // ns

} // ns
