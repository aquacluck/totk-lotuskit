#pragma once

namespace engine::transceiver {

    class TransceiverBase {
        public:
        virtual ~TransceiverBase(); // vtable
        u32 mTransceiverIndex;
        u32 mChannelMask;
        // XXX TransceiverTreeMapNode mTreeMapNode;
        void* idk; // TransceiverTreeMapNode vtable?
        char mTreeMapNode[0x30];
    };

} // ns
