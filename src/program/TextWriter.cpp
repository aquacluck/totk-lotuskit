#include "TextWriter.hpp"

namespace lotuskit {
    namespace TextWriterHooks {
        GraphicsModuleCreateArg GetCreateArg::create_arg = {0};
    } // ns

    TextWriterDrawFrame TextWriter::drawFrames[2] = {};

    TextWriterDrawNode* TextWriter::appendNewDrawNode(size_t drawList_i) {
        // alloc
        TextWriterDrawFrame* frame = currentDrawFrame;
        TextWriterDrawNode* newNode = (TextWriterDrawNode*)frame->heap->alloc(sizeof(TextWriterDrawNode));
        newNode->outputText = nullptr;
        newNode->fn = nullptr;
        newNode->next.store(nullptr);

        // append
        TextWriterDrawNode* cmpNode = nullptr; // ensure null at time of write
        if (frame->drawLists[drawList_i].compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success -- appended to empty list
        // not null, enter the list
        TextWriterDrawNode* node = frame->drawLists[drawList_i].load();
        while (true) {
            cmpNode = nullptr; // ensure null at time of write
            if (node->next.compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success
            // not null, traverse into next
            node = cmpNode;
        }

        return nullptr; // unreachable
    }

    void TextWriter::drawFrame(TextWriterExt* writer) {
        TextWriterDrawFrame* frame = currentDrawFrame;
        currentDrawFrame = currentDrawFrame == drawFrames ? &(drawFrames[1]) : drawFrames; // swap any further calls to the other buffer
        // XXX is it possible an enqueue callsite allocs then gets stalled long enough to write freed ptrs to an emptied TextWriterDrawFrame list?

        for (size_t i=0; i < TextWriterDrawFrame::MAX_DRAWLISTS; i++) {
            TextWriterDrawNode* node = frame->drawLists[i].load();
            if (node == nullptr) { continue; }

            sead::Vector2f textPos; // screen is always represented as 1280x720, upscaled for 1080p
            // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
            writer->mScale.x = 0.8;
            writer->mScale.y = 0.8;
            textPos.x = 2.0;
            textPos.y = 2.0;

            do {
                if (node->fn) {
                    node->fn(writer, &textPos);
                }
                if (node->outputText != nullptr) {
                    writer->pprintf(textPos, node->outputText);
                }
                node = node->next.load();

            } while (node != nullptr);

            frame->drawLists[i].store(nullptr);
        }

        frame->heap->freeAll();
    }

    void TextWriter::drawToasts(TextWriterExt* writer) {
        sead::Vector2f textPos; // screen is always represented as 1280x720, upscaled for 1080p
        // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
        writer->mScale.x = 0.8;
        writer->mScale.y = 0.8;
        textPos.x = 1280.0 - 500.0; // mid right ish
        textPos.y = 128.0;

        u64 nowSystemTick = svcGetSystemTick();
        for (size_t i=0; i < TextWriter::MAX_TOASTS; i++) {
            TextWriterToastNode* node = toasts[i].load();
            if (node == nullptr) { continue; }
            if (nowSystemTick > node->expirySystemTick) {
                toasts[i].store(nullptr);
                if (node->outputText != nullptr) {
                    debugDrawerInternalHeap->free(node->outputText);
                }
                debugDrawerInternalHeap->free(node);
                continue;
            }
            if (node->fn) {
                node->fn(writer, &textPos);
            }
            if (node->outputText != nullptr) {
                writer->pprintf(textPos, node->outputText);
            }
        }
    }

} // ns
