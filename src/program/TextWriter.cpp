#include "TextWriter.hpp"

namespace lotuskit {
    TextWriterFrame TextWriter::frame = {};

    void TextWriter::appendNewDrawNode(size_t drawList_i, const char* text, TextWriterDrawCallback* fn, float scale, const sead::Color4f* color) {
        if (frame.heap == nullptr) { return; } // uninitialized
        TextWriterDrawNode* newNode = nullptr;
        TextWriterDrawNode* cmpNode = nullptr;
        TextWriterDrawNode* node = nullptr; // for traversal

        // drop concurrent + mid-draw requests
        // TODO lock free appends are possible (although current impl may be bad under contention?), but unclear how lock free alloc+freeAll would work (eg FrameHeap behavior)
        if (!nn::os::TryLockMutex(&frame.drawListLock)) { return; } // lock or fail

        // alloc + store
        newNode = (TextWriterDrawNode*)frame.heap->alloc(sizeof(TextWriterDrawNode));
        if (newNode == nullptr) { goto RELEASE_AND_RETURN; } // alloc fail (eg too many writes piled up)
        newNode->outputText = nullptr;
        newNode->fn = fn; // fn lifetime managed by caller
        newNode->scale = scale;
        newNode->color = color ? *color : sead::Color4f{0.85, 0.85, 0.85, 1.0}; // white ish
        newNode->next.store(nullptr);
        if (text != nullptr) {
            auto n = strlen(text) + 1;
            newNode->outputText = (char*)frame.heap->alloc(n);
            // don't even bother appending partially allocated nodes, just drop the node/call
            if (newNode->outputText == nullptr) { goto RELEASE_AND_RETURN; }
            std::memcpy(newNode->outputText, text, n);
        }

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
        nn::os::UnlockMutex(&frame.drawListLock); // release
    }

    void TextWriter::drawFrame(TextWriterExt* writer) {
        nn::os::LockMutex(&frame.drawListLock);

        for (size_t i=0; i < TextWriterFrame::MAX_DRAWLISTS; i++) {
            TextWriterDrawNode* node = frame.drawLists[i].load();
            if (node == nullptr) { continue; }

            sead::Vector2f textPos; // screen is always represented as 1280x720, upscaled for 1080p
            textPos.x = 2.0;
            textPos.y = 2.0;

            do {
                if (node->scale == 0.0) {
                    // default
                    // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
                    writer->mScale.x = 0.8;
                    writer->mScale.y = 0.8;
                } else if (node->scale == -1.0) {
                    // no change
                } else {
                    // assign
                    writer->mScale.x = node->scale;
                    writer->mScale.y = node->scale;
                }

                if (node->fn != nullptr) {
                    node->fn(writer, &textPos);
                }
                if (node->outputText != nullptr) {
                    writer->pprintf(textPos, node->color, node->outputText);
                }

                node = node->next.load();
            } while (node != nullptr);

            frame.drawLists[i].store(nullptr); // wipe for next frame
        }

        frame.heap->freeAll();
        nn::os::UnlockMutex(&frame.drawListLock);
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
                auto color = sead::Color4f{0.85, 0.85, 0.85, 1.0}; // white ish TODO toast color/scale?
                writer->pprintf(textPos, color, node->outputText);
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
            if (toasts[i].compare_exchange_strong(cmpNode, newNode)) { return newNode; } // success -- inserted
        }

        // fail
        Logger::logText("[ERROR] toast overflow", "/TextWriter");
        return nullptr;
    }

} // ns
