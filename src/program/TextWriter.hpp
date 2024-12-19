#pragma once
#include <atomic>
#include "lib.hpp"
#include "syms_merged.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

#include "nn/nn.h"
#include "nn/util.h"
#include "heap/seadHeap.h"
#include "heap/seadFrameHeap.h"
#include "math/seadVector.hpp"

// agl+gfx include
#include "common/aglDrawContext.h"
#include "lyr/aglLayer.h"
#include "lyr/aglRenderInfo.h"
#include "gfx/seadColor.h"
#include "gfx/seadDrawContext.h"
#include "gfx/seadTextWriter.h"


namespace lotuskit {
    class TextWriterExt;
    using TextWriterDrawCallback = void(*)(TextWriterExt*, sead::Vector2f*);

    struct TextWriterDrawNode {
        // nodes drawn sequentially, invoking fn if set (eg for positioning) and drawing outputText if set
        char* outputText;
        TextWriterDrawCallback fn;
        std::atomic<TextWriterDrawNode*> next;
    };

    struct TextWriterDrawFrame {
        static constexpr size_t MAX_DRAWLISTS = 8;
        inline static std::atomic<TextWriterDrawNode*> drawLists[MAX_DRAWLISTS] = {0}; // independent chains of calls (eg for different relative positionings)
        inline static sead::FrameHeap* heap = nullptr; // draw state is wiped each frame
    };

    struct TextWriterToastNode {
        char* outputText;
        TextWriterDrawCallback fn;
        u64 expirySystemTick;
    };

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
        inline static void printf(size_t drawList_i, const char* fmt, auto&&... args) {
            // FIXME some symbol export issue if placed in cpp? [rtld] Unresolved symbol: _ZN8lotuskit10TextWriter6printfIJRA4_KcEEEvmPS2_DpOT_
            // FIXME early return if textwriter disabled?
            TextWriterDrawNode* newNode = appendNewDrawNode(drawList_i);

            // set outputText
            char buf[2000]; // TODO std::format etc with proper allocator?
            nn::util::SNPrintf(buf, sizeof(buf), fmt, std::forward<decltype(args)>(args)...);
            newNode->outputText = (char*)currentDrawFrame->heap->alloc(strlen(buf)+1);
            std::memcpy(newNode->outputText, buf, strlen(buf)+1);
        }
        inline static void appendCallback(size_t drawList_i, TextWriterDrawCallback fn) {
            TextWriterDrawNode* newNode = appendNewDrawNode(drawList_i);
            newNode->fn = fn;
        }
        inline static void toastf(u64 ttl30, const char* fmt, auto&&... args) {
            TextWriterToastNode* newNode = (TextWriterToastNode*)debugDrawerInternalHeap->alloc(sizeof(TextWriterToastNode));
            newNode->outputText = nullptr;
            newNode->fn = nullptr;
            // FIXME just store ttl30 and count down frames??? why did i do this?
            newNode->expirySystemTick = ttl30 * 1000000 + svcGetSystemTick(); // FIXME approximate conversion, doesnt matter much

            // set outputText
            char buf[2000]; // TODO std::format etc with proper allocator?
            nn::util::SNPrintf(buf, sizeof(buf), fmt, std::forward<decltype(args)>(args)...);
            newNode->outputText = (char*)debugDrawerInternalHeap->alloc(strlen(buf)+1);
            std::memcpy(newNode->outputText, buf, strlen(buf)+1);

            TextWriterToastNode* cmpNode;
            for(size_t i=0; i < MAX_TOASTS; i++) {
                //if (toasts[i] == nullptr) { toasts[i] = newNode; }
                cmpNode = nullptr; // ensure null at time of write
                if (toasts[i].compare_exchange_weak(cmpNode, newNode)) { return; } // success -- inserted
            }

            // fail
            Logger::logText("[ERROR] toast overflow", "/TextWriter");
        }

        // toast storage: repeated draw calls for some duration, not part of any drawlist
        static constexpr size_t MAX_TOASTS = 0x20; // best not to go crazy on these, there's no way to scroll besides staggered ttl anyways
        inline static std::atomic<TextWriterToastNode*> toasts[MAX_TOASTS] = {0};

        // buffer+swap+draw impl
        static TextWriterDrawFrame drawFrames[2];
        inline static TextWriterDrawFrame* currentDrawFrame = nullptr;
        inline static sead::Heap* debugDrawerInternalHeap = nullptr; // idk what really happens in here yet
        inline static void assignHeap(sead::Heap* heap) {
            debugDrawerInternalHeap = heap;
            currentDrawFrame = drawFrames;
            drawFrames[0].heap = sead::FrameHeap::create(0x4000, "lotuskit::TextWriterDrawFrame[0]", heap, 8, sead::Heap::cHeapDirection_Forward, 0); // XXX what are args 4+6?
            drawFrames[1].heap = sead::FrameHeap::create(0x4000, "lotuskit::TextWriterDrawFrame[1]", heap, 8, sead::Heap::cHeapDirection_Forward, 0);
            //debugDrawCallHeap->alloc(32); debugDrawCallHeap->freeAll();
        }
        static TextWriterDrawNode* appendNewDrawNode(size_t drawList_i);
        static void drawFrame(TextWriterExt*);
        static void drawToasts(TextWriterExt*);
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
                        InitDebugDrawers(TextWriter::debugDrawerInternalHeap, GetCreateArg::create_arg);

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

                lotuskit::TextWriter::drawFrame(writer);
                lotuskit::TextWriter::drawToasts(writer);
            }
        };

    } // ns

} // ns
