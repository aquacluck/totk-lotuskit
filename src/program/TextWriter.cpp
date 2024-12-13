#include "TextWriter.hpp"

namespace lotuskit {
    namespace TextWriterHooks {
        GraphicsModuleCreateArg GetCreateArg::create_arg = {0};
    } // ns

    TextWriterDrawFrame TextWriter::drawFrames[2] = {};

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
                    node->fn(writer); // TODO textPos
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

} // ns
