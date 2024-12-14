#pragma once
#include <cstring>
#include "TextWriter.hpp"

namespace lotuskit {
    class HexDumpEntry {
        public:
        static constexpr u32 BUF_LEN = 0x1000; // 4KB
        bool isCalc; // observe dumpSrc, to eg TODO watch/search and report to the frontend without drawing
        bool isDraw; // textwriter output
        // TODO accept+store nso module offsets, sead::Heap* relative, etc traversal for certain container types
        // srcTypeId = absolute
        void* dumpSrc; // absolute address to inspect TODO derive from ^. This is always the "beginning" of the observed region for any pagination/etc logic, even for negative
        u8 buf[BUF_LEN]; // every calc will replace the contents of this buffer, perhaps performing comparisons/etc first
        s32 dumpLen; // abs(dumpLen) capped at BUF_LEN, but +-2GB addressable. Should be a multiple of 0x10
        u16 drawLen; // capped at smallest of: abs(dumpLen) or BUF_LEN or about a screenful, to eg observe a large BUF_LEN but limit the output. default: about a screenful. Should be a multiple of 0x10
        u32 calcAge; // calc ticks since isCalc was last true / buf was refreshed
        char label[33];

        void clear() {
            this->isCalc = false;
            this->isDraw = false;
            //this->srcTypeId = absolute;
            this->dumpSrc = nullptr;
            this->dumpLen = 0x20;
            this->drawLen = 0x20;
            this->calcAge = 0;
            strcpy(this->label, "HexDump");
        }
    };

    class HexDump {
        public:
        static constexpr size_t MAX_DUMP_SLOTS = 4;
        inline static HexDumpEntry slots[MAX_DUMP_SLOTS] = {0};

        inline static void clearSlot(size_t i) { slots[i].clear(); }
        inline static void pauseSlot(size_t i) { slots[i].isCalc = false; }
        inline static void assignSlotAbsolute(size_t i, void* dumpSrc, u32 dumpLen, u32 drawLen) {
            auto& slot = slots[i];
            slot.clear();
            //strncpy(cmd.label, label, 33); // TODO AS string type
            slot.dumpLen = dumpLen;
            slot.drawLen = drawLen;
            slot.isCalc = true;
            slot.dumpSrc = dumpLen >= 0 ? dumpSrc : dumpSrc + dumpLen; // negative lengths aim behind ptr
        }

        inline static void calc() {
            for (u8 i=0; i < MAX_DUMP_SLOTS; i++) {
                auto& slot = slots[i];
                if (!slot.isCalc || slot.dumpSrc == nullptr) {
                    slot.calcAge++;
                } else {
                    // dump into buffer
                    u32 len = abs(slot.dumpLen);
                    len = len > HexDumpEntry::BUF_LEN ? HexDumpEntry::BUF_LEN : len;
                    std::memcpy(slot.buf, slot.dumpSrc, len);

                    slot.calcAge = 0;
                    slot.isDraw = true; // XXX maybe we don't always want to draw?
                }

                if (slot.isDraw) {
                    const char* live = slot.calcAge == 0 ? "[LIVE]" : ""; // visible when buf has been updated this frame, for slow/intermittent/triggered/discontinued/etc dumps
                    lotuskit::TextWriter::printf(0, "%s[%d](dumpLen=%d, age=%d) %s\r\n", slot.label, i, slot.dumpLen, slot.calcAge, live);

                    u32 cap = reinterpret_cast<u32>((u32)slot.dumpLen > HexDumpEntry::BUF_LEN ? HexDumpEntry::BUF_LEN : slot.dumpLen);
                    u32 drawLen = slot.drawLen > cap ? cap : slot.drawLen;

                    u8* backingSrc = (u8*)slot.dumpSrc;
                    u8* src = slot.buf;
                    u32 row = 0;
                    do { if (row * 0x10 >= drawLen) { break; }

                        lotuskit::TextWriter::printf(0,
                            "%p | %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
                            backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15]
                        );

                        row++;
                        backingSrc += 0x10;
                        src += 0x10;
                    } while (true);
                }
            }
        }

    };
}
