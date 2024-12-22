#include "TextWriter.hpp"

namespace lotuskit {
    DebugDrawFrame TextWriter::drawFrames[2] = {};

    TextWriterDrawNode* TextWriter::appendNewDrawNode(size_t drawList_i) {
        // alloc
        DebugDrawFrame* frame = currentDrawFrame;
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
        DebugDrawFrame* frame = currentDrawFrame;

        for (size_t i=0; i < DebugDrawFrame::MAX_DRAWLISTS; i++) {
            TextWriterDrawNode* node = frame->drawLists[i].load();
            if (node == nullptr) { continue; }

            sead::Vector2f textPos; // screen is always represented as 1280x720, upscaled for 1080p
            // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
            writer->mScale.x = 0.8;
            writer->mScale.y = 0.8;
            textPos.x = 2.0;
            textPos.y = 2.0;

            do {
                if (node->fn != nullptr) {
                    node->fn(writer, &textPos);
                }
                if (node->outputText != nullptr) {
                    writer->pprintf(textPos, node->outputText);
                }
                node = node->next.load();

            } while (node != nullptr);

        }
    }

    void TextWriter::drawToasts(TextWriterExt* writer) {
        sead::Vector2f textPos; // screen is always represented as 1280x720, upscaled for 1080p
        // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
        writer->mScale.x = 0.8;
        writer->mScale.y = 0.8;
        textPos.x = 1280.0 - 500.0; // mid right ish
        textPos.y = 128.0;

        for (size_t i=0; i < TextWriter::MAX_TOASTS; i++) {
            TextWriterToastNode* node = toasts[i].load();
            if (node == nullptr) { continue; }
            if (node->ttlFrames-- <= 0) {
                toasts[i].store(nullptr);
                if (node->outputText != nullptr) {
                    debugDrawerInternalHeap->free(node->outputText);
                }
                //if (node->primCallType != 0 && node->primCallArgs != nullptr) {
                //    debugDrawerInternalHeap->free(node->primCallArgs);
                //}
                debugDrawerInternalHeap->free(node);
                continue;
            }
            if (node->fn != nullptr) {
                node->fn(writer, &textPos);
            }
            if (node->outputText != nullptr) {
                writer->pprintf(textPos, node->outputText);
            }
            //if (node->primCallType != 0) {
            //    PrimitiveImpl::dispatch(node->primCallType, node->primCallArgs);
            //}
        }
    }

    TextWriterToastNode* TextWriter::appendNewToastNode(u64 ttlFrames) {
        TextWriterToastNode* newNode = (TextWriterToastNode*)debugDrawerInternalHeap->alloc(sizeof(TextWriterToastNode));
        newNode->outputText = nullptr;
        newNode->fn = nullptr;
        //newNode->primCallType = 0;
        //newNode->primCallArgs = nullptr;
        newNode->ttlFrames = ttlFrames;

        TextWriterToastNode* cmpNode;
        for(size_t i=0; i < MAX_TOASTS; i++) {
            //if (toasts[i] == nullptr) { toasts[i] = newNode; }
            cmpNode = nullptr; // ensure null at time of write
            if (toasts[i].compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success -- inserted
        }

        // fail
        Logger::logText("[ERROR] toast overflow", "/TextWriter");
        return nullptr;
    }

    void TextWriter::swapFrame() {
        DebugDrawFrame* frame = currentDrawFrame;

        // swap any further calls to the other buffer
        currentDrawFrame = currentDrawFrame == drawFrames ? &(drawFrames[1]) : drawFrames;

        frame->heap->freeAll();
        for (size_t i=0; i < DebugDrawFrame::MAX_DRAWLISTS; i++) {
            // ensure lists are empty even if the contents were not actually drawn
            frame->drawLists[i].store(nullptr);
            frame->drawLists3d[i].store(nullptr);
        }
    }

} // ns
