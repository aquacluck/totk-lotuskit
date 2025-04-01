#pragma once
#include <string>
#include "TextWriter.hpp"
#include "util/disasm.hpp"

namespace lotuskit {
    enum class HexDumpDataType: u32 {
        U8=0,
        U16,
        U32,
        U64,
        S8,
        S16,
        S32,
        S64,
        FLOAT,
        DOUBLE,
        TEXT, // u8 + text sidebar
        CODE_AARCH64, // u8 + TODO disasm
    };

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
        u32 drawLen; // capped at smallest of: abs(dumpLen) or BUF_LEN or about a screenful, to eg observe a large BUF_LEN but limit the output. default: about a screenful. Should be a multiple of 0x10
        u32 calcAge; // calc ticks since isCalc was last true / buf was refreshed
        std::string label; // user chosen for reference
        HexDumpDataType dataType; // try to format data to this type
        bool formatHex; // prefer hex over decimal (when applicable)

        void clear() {
            this->isCalc = false;
            this->isDraw = false;
            //this->srcTypeId = absolute;
            this->dumpSrc = nullptr;
            this->dumpLen = 0x20;
            this->drawLen = 0x20;
            this->calcAge = 0;
            this->label = "HexDump";
            this->dataType = HexDumpDataType::U8;
            this->formatHex = true;
        }
    };

    class HexDump {
        public:
        static constexpr size_t MAX_DUMP_SLOTS = 4;
        inline static HexDumpEntry slots[MAX_DUMP_SLOTS] = {0};

        inline static void clearSlot(size_t i) { slots[i].clear(); }
        inline static void pauseSlot(size_t i) { slots[i].isCalc = false; }
        inline static void assignSlotAbsolute(size_t i, void* dumpSrc, u32 dumpLen, u32 drawLen, const std::string &label, u32 dataType, bool formatHex) {
            auto& slot = slots[i];
            slot.clear();
            slot.label = label;
            slot.dumpLen = dumpLen;
            slot.drawLen = drawLen;
            slot.dataType = HexDumpDataType(dataType);
            slot.formatHex = formatHex;
            if (dumpSrc == nullptr) {
                lotuskit::TextWriter::toastf(30*5, "[error] nullptr HexDump requested for %s[%d]\n", label.c_str(), i);
                return;
            }
            slot.isCalc = true;
            slot.dumpSrc = dumpLen >= 0 ? dumpSrc : dumpSrc + dumpLen; // negative lengths aim behind ptr
        }

        inline static void textwriter_printf_u8(size_t drawList_i, void* backingSrc, u8* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x\n";
            const char* fmtDec = "%p | %u %u %u %u %u %u %u %u  %u %u %u %u %u %u %u %u\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15]
                );
                backingSrc += 0x10;
                src += 0x10;
            }
        }
        inline static void textwriter_printf_u16(size_t drawList_i, void* backingSrc, u16* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %04x %04x %04x %04x  %04x %04x %04x %04x\n";
            const char* fmtDec = "%p | %u %u %u %u  %u %u %u %u\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7]
                );
                backingSrc += 0x10;
                src += 8; // 0x10
            }
        }
        inline static void textwriter_printf_u32(size_t drawList_i, void* backingSrc, u32* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %08x %08x %08x %08x\n";
            const char* fmtDec = "%p | %u %u %u %u\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3]
                );
                backingSrc += 0x10;
                src += 4; // 0x10
            }
        }
        inline static void textwriter_printf_u64(size_t drawList_i, void* backingSrc, u64* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %p %p\n";
            const char* fmtDec = "%p | %llu %llu\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1]
                );
                backingSrc += 0x10;
                src += 2; // 0x10
            }
        }
        inline static void textwriter_printf_s8(size_t drawList_i, void* backingSrc, s8* src, u32 lines=1, bool formatHex=true) {
            // XXX signed hex is kinda pointless, and painful to print, so lets just print it raw? idk
            const char* fmtHex = "%p | %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x\n";
            const char* fmtDec = "%p | %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15]
                );
                backingSrc += 0x10;
                src += 0x10;
            }
        }
        inline static void textwriter_printf_s16(size_t drawList_i, void* backingSrc, s16* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %04x %04x %04x %04x  %04x %04x %04x %04x\n";
            const char* fmtDec = "%p | %d %d %d %d  %d %d %d %d\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7]
                );
                backingSrc += 0x10;
                src += 8; // 0x10
            }
        }
        inline static void textwriter_printf_s32(size_t drawList_i, void* backingSrc, s32* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %08x %08x %08x %08x\n";
            const char* fmtDec = "%p | %d %d %d %d\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3]
                );
                backingSrc += 0x10;
                src += 4; // 0x10
            }
        }
        inline static void textwriter_printf_s64(size_t drawList_i, void* backingSrc, s64* src, u32 lines=1, bool formatHex=true) {
            const char* fmtHex = "%p | %p %p\n";
            const char* fmtDec = "%p | %lld %lld\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1]
                );
                backingSrc += 0x10;
                src += 2; // 0x10
            }
        }
        inline static void textwriter_printf_float(size_t drawList_i, void* backingSrc, float* src, u32 lines=1, bool formatHex=false) {
            // const char* fmtHex = "%p | %a %a %a %a\n"; // XXX dont work, idk what it would do anyways lol
            const char* fmtDec = "%p | %f %f %f %f\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3]
                );
                backingSrc += 0x10;
                src += 4; // 0x10
            }
        }
        inline static void textwriter_printf_double(size_t drawList_i, void* backingSrc, double* src, u32 lines=1, bool formatHex=false) {
            // const char* fmtHex = "%p | %a %a\n"; // XXX dont work, idk what it would do anyways lol
            const char* fmtDec = "%p | %f %f\n";
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    fmtDec,
                    backingSrc, src[0], src[1]
                );
                backingSrc += 0x10;
                src += 2; // 0x10
            }
        }

        inline static void textwriter_printf_text(size_t drawList_i, void* backingSrc, void* src_, u32 lines=1, bool formatHex=true) {
            // side by side `hexdump -C` style hex+printables
            const char* fmtHex = "%p | %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x  |%s|\n";
            const char* fmtDec = "%p | %u %u %u %u %u %u %u %u  %u %u %u %u %u %u %u %u  |%s|\n";
            u8* src = (u8*)src_;
            char txt[17]; txt[16] = 0;
            while (lines--) {
                for (int i=15; i >= 0; i--) {
                    txt[i] = std::isprint(src[i]) ? src[i] : '.';
                }
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmtHex : fmtDec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15], txt
                );
                backingSrc += 0x10;
                src += 0x10;
            }
        }

        inline static void textwriter_printf_code_aarch64(size_t drawList_i, void* backingSrc, void* src_, u32 lines=1, bool formatHex=false) {
            u8* src = (u8*)src_;
            const auto n = lotuskit::util::disasm::MNM_OUT_MAX_SIZE;
            char mnm[n+1]; mnm[n] = 0;
            s64 lineMainOffset = (s64)(backingSrc - exl::util::GetMainModuleInfo().m_Total.m_Start);
            while (lines--) {
                u32 inst = *((u32*)src);
                lotuskit::util::disasm::u32_to_str_mnemonic(inst, (char*)mnm);
                if (lineMainOffset >= 0 && lineMainOffset <= 0xffffffff && !formatHex) {
                    // formatHex inhibits main+offset formatting
                    lotuskit::TextWriter::printf(drawList_i,
                        "main+%08x | %02x %02x %02x %02x  %s\n",
                        lineMainOffset, src[0], src[1], src[2], src[3], (char*)mnm
                    );
                } else {
                    lotuskit::TextWriter::printf(drawList_i,
                        "%p | %02x %02x %02x %02x  %s\n",
                        backingSrc, src[0], src[1], src[2], src[3], (char*)mnm
                    );
                }
                lineMainOffset += 4;
                backingSrc += 4;
                src += 4;
            }
        }

        inline static void calc() {
            for (u8 i=0; i < MAX_DUMP_SLOTS; i++) {
                auto& slot = slots[i];
                if (!slot.isCalc || slot.dumpSrc == nullptr) {
                    slot.calcAge++;
                } else {
                    // dump into buffer
                    const u32 dumpLen = std::min(static_cast<u32>(abs(slot.dumpLen)), HexDumpEntry::BUF_LEN);
                    std::memcpy(slot.buf, slot.dumpSrc, dumpLen);

                    slot.calcAge = 0;
                    slot.isDraw = true; // XXX maybe we don't always want to draw?
                }

                if (slot.isDraw) {
                    const char* live = slot.calcAge == 0 ? "[LIVE]" : ""; // visible when buf has been updated this frame, for slow/intermittent/triggered/discontinued/etc dumps
                    lotuskit::TextWriter::printf(0, "%s[%d](dumpLen=%d, age=%d) %s\r\n", slot.label.c_str(), i, slot.dumpLen, slot.calcAge, live);

                    const u32 drawMax = std::min(static_cast<u32>(abs(slot.dumpLen)), HexDumpEntry::BUF_LEN);
                    const u32 drawLen = std::min(slot.drawLen, drawMax);

                    void* backingSrc = slot.dumpSrc;
                    void* src = slot.buf;
                    u32 row = 0;
                    while(true) {
                        if (row * 0x10 >= drawLen) { break; }

                        if (       slot.dataType == HexDumpDataType::U8) {
                            textwriter_printf_u8(0, backingSrc, (u8*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::U16) {
                            textwriter_printf_u16(0, backingSrc, (u16*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::U32) {
                            textwriter_printf_u32(0, backingSrc, (u32*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::U64) {
                            textwriter_printf_u64(0, backingSrc, (u64*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S8) {
                            textwriter_printf_s8(0, backingSrc, (s8*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S16) {
                            textwriter_printf_s16(0, backingSrc, (s16*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S32) {
                            textwriter_printf_s32(0, backingSrc, (s32*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S64) {
                            textwriter_printf_s64(0, backingSrc, (s64*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::FLOAT) {
                            textwriter_printf_float(0, backingSrc, (float*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::DOUBLE) {
                            textwriter_printf_double(0, backingSrc, (double*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::TEXT) {
                            textwriter_printf_text(0, backingSrc, src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::CODE_AARCH64) {
                            // code is formatted 4B per line, so 0x10 stride is split into 4 output lines
                            textwriter_printf_code_aarch64(0, backingSrc, src, 4, slot.formatHex);
                        }

                        row++;
                        backingSrc += 0x10;
                        src += 0x10;
                    }
                }
            }
        }

    };
}
