#include "TextWriter.hpp"

namespace lotuskit {
    TextWriterFrame TextWriter::frame = {0};

    TextWriterDrawNode* TextWriter::appendNewDrawNode(size_t drawList_i) {
        // alloc
        if (frame.heap == nullptr) { return nullptr; }
        TextWriterDrawNode* newNode = (TextWriterDrawNode*)frame.heap->alloc(sizeof(TextWriterDrawNode));
        if (newNode == nullptr) { return nullptr; } // heap full (this could happen if Tool 2D was blocked from drawing)
        newNode->outputText = nullptr;
        newNode->fn = nullptr;
        newNode->next.store(nullptr);

        // append
        TextWriterDrawNode* cmpNode = nullptr; // ensure null at time of write
        if (frame.drawLists[drawList_i].compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success -- appended to empty list
        // not null, enter the list
        TextWriterDrawNode* node = frame.drawLists[drawList_i].load();
        while (node) {
            cmpNode = nullptr; // ensure null at time of write // FIXME can't node be freed rn?
            if (node->next.compare_exchange_weak(cmpNode, newNode)) { return newNode; } // success
            // not null, traverse into next
            node = cmpNode;
        }

        return nullptr; // XXX frame completed after first compare, just abandon this draw node, its data is probably stale anyways
    }

    void TextWriter::drawFrame(TextWriterExt* writer) {
        nn::os::LockMutex(&frame.drawLock); // FIXME useless locking

        for (size_t i=0; i < TextWriterFrame::MAX_DRAWLISTS; i++) {
            TextWriterDrawNode* node = frame.drawLists[i].load();
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

            frame.drawLists[i].store(nullptr); // wipe for next frame
        }

        frame.heap->freeAll();
        nn::os::UnlockMutex(&frame.drawLock);
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
            if (node->ttlFrames == 0) {
                toasts[i].store(nullptr);
                if (node->outputText != nullptr) {
                    debugDrawerInternalHeap->free(node->outputText);
                }
                debugDrawerInternalHeap->free(node);
                continue;
            }
            node->ttlFrames--;
            if (node->fn != nullptr) {
                node->fn(writer, &textPos);
            }
            if (node->outputText != nullptr) {
                writer->pprintf(textPos, node->outputText);
            }
        }
    }

    TextWriterToastNode* TextWriter::appendNewToastNode(u32 ttlFrames) {
        TextWriterToastNode* newNode = (TextWriterToastNode*)debugDrawerInternalHeap->alloc(sizeof(TextWriterToastNode));
        newNode->outputText = nullptr;
        newNode->fn = nullptr;
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

} // ns
