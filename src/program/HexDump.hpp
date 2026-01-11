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
        inline static constexpr u32 BUF_LEN = 0x1000; // 4KB
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

        inline void clear() {
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
        inline static constexpr size_t MAX_DUMP_SLOTS = 4;
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

        inline static const char* fmt_u8_hex = "%p | %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x\n";
        inline static const char* fmt_u8_dec = "%p | %u %u %u %u  %u %u %u %u  %u %u %u %u  %u %u %u %u\n";
        inline static const char* fmt_u16_hex = "%p | %04x %04x  %04x %04x  %04x %04x  %04x %04x\n";
        inline static const char* fmt_u16_dec = "%p | %u %u  %u %u  %u %u  %u %u\n";
        inline static const char* fmt_u32_hex = "%p | %08x %08x %08x %08x\n";
        inline static const char* fmt_u32_dec = "%p | %u %u %u %u\n";
        inline static const char* fmt_u64_hex = "%p | %p %p\n";
        inline static const char* fmt_u64_dec = "%p | %llu %llu\n";
        inline static const char* fmt_s8_hex = "%p | %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x\n";
        inline static const char* fmt_s8_dec = "%p | %d %d %d %d  %d %d %d %d  %d %d %d %d  %d %d %d %d\n";
        inline static const char* fmt_s16_hex = "%p | %04x %04x  %04x %04x  %04x %04x  %04x %04x\n";
        inline static const char* fmt_s16_dec = "%p | %d %d  %d %d  %d %d  %d %d\n";
        inline static const char* fmt_s32_hex = "%p | %08x %08x %08x %08x\n";
        inline static const char* fmt_s32_dec = "%p | %d %d %d %d\n";
        inline static const char* fmt_s64_hex = "%p | %p %p\n";
        inline static const char* fmt_s64_dec = "%p | %lld %lld\n";
        //inline static const char* fmt_float_hex = "%p | %a %a %a %a\n"; // TODO show float bits/info?
        inline static const char* fmt_float_dec = "%p | %f %f %f %f\n";
        //inline static const char* fmt_double_hex = "%p | %a %a\n"; // TODO show float bits/info?
        inline static const char* fmt_double_dec = "%p | %f %f\n";
        inline static const char* fmt_text_hex = "%p | %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  |%s|\n";
        inline static const char* fmt_text_dec = "%p | %u %u %u %u  %u %u %u %u  %u %u %u %u  %u %u %u %u  |%s|\n";
        inline static const char* fmt_code_aarch64_hex = "%p | %02x %02x %02x %02x  %s\n";
        inline static const char* fmt_code_aarch64_dec = "%s+%08x | %02x %02x %02x %02x  %s\n";

        inline static void print_u8(s64 drawList_i, void* backingSrc, u8* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_u8_hex : fmt_u8_dec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15]
                );
                backingSrc += 0x10;
                src += 0x10;
            }
        }
        inline static void print_u16(s64 drawList_i, void* backingSrc, u16* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_u16_hex : fmt_u16_dec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7]
                );
                backingSrc += 0x10;
                src += 8; // 0x10
            }
        }
        inline static void print_u32(s64 drawList_i, void* backingSrc, u32* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_u32_hex : fmt_u32_dec,
                    backingSrc, src[0], src[1], src[2], src[3]
                );
                backingSrc += 0x10;
                src += 4; // 0x10
            }
        }
        inline static void print_u64(s64 drawList_i, void* backingSrc, u64* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_u64_hex : fmt_u64_dec,
                    backingSrc, src[0], src[1]
                );
                backingSrc += 0x10;
                src += 2; // 0x10
            }
        }
        inline static void print_s8(s64 drawList_i, void* backingSrc, s8* src, u32 lines=1, bool formatHex=true) {
            // XXX signed hex is kinda pointless, and painful to print, so lets just print it raw? idk
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_s8_hex : fmt_s8_dec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15]
                );
                backingSrc += 0x10;
                src += 0x10;
            }
        }
        inline static void print_s16(s64 drawList_i, void* backingSrc, s16* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_s16_hex : fmt_s16_dec,
                    backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7]
                );
                backingSrc += 0x10;
                src += 8; // 0x10
            }
        }
        inline static void print_s32(s64 drawList_i, void* backingSrc, s32* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_s32_hex : fmt_s32_dec,
                    backingSrc, src[0], src[1], src[2], src[3]
                );
                backingSrc += 0x10;
                src += 4; // 0x10
            }
        }
        inline static void print_s64(s64 drawList_i, void* backingSrc, s64* src, u32 lines=1, bool formatHex=true) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    formatHex ? fmt_s64_hex : fmt_s64_dec,
                    backingSrc, src[0], src[1]
                );
                backingSrc += 0x10;
                src += 2; // 0x10
            }
        }
        inline static void print_float(s64 drawList_i, void* backingSrc, float* src, u32 lines=1, bool formatHex=false) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    fmt_float_dec,
                    backingSrc, src[0], src[1], src[2], src[3]
                );
                backingSrc += 0x10;
                src += 4; // 0x10
            }
        }
        inline static void print_double(s64 drawList_i, void* backingSrc, double* src, u32 lines=1, bool formatHex=false) {
            while (lines--) {
                lotuskit::TextWriter::printf(drawList_i,
                    fmt_double_dec,
                    backingSrc, src[0], src[1]
                );
                backingSrc += 0x10;
                src += 2; // 0x10
            }
        }

        inline static void print_text(s64 drawList_i, void* backingSrc, void* src_, u32 lines=1, bool formatHex=true) {
            // side by side `hexdump -C` style hex+printables
            u8* src = (u8*)src_;
            char txt[17]; txt[16] = 0;
            while (lines--) {
                for (int i=15; i >= 0; i--) {
                    txt[i] = std::isprint(src[i]) ? src[i] : '.';
                }
                if (drawList_i == -1) {
                    // TODO update other printers as needed for dual use, drawlist=-1 as debug log seems nice
                    char buf[200];
                    nn::util::SNPrintf(buf, sizeof(buf),
                        formatHex ? fmt_text_hex : fmt_text_dec,
                        backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15], txt
                    );
                    svcOutputDebugString(buf, strlen(buf)-1); // no newline
                } else {
                    lotuskit::TextWriter::printf(drawList_i,
                        formatHex ? fmt_text_hex : fmt_text_dec,
                        backingSrc, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15], txt
                    );
                }
                backingSrc += 0x10;
                src += 0x10;
            }
        }

        inline static void print_code_aarch64(s64 drawList_i, void* backingSrc, void* src_, u32 lines=1, bool formatHex=false) {
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
                        "%s+%08x | %02x %02x %02x %02x  %s\n",
                        "main", lineMainOffset, src[0], src[1], src[2], src[3], (char*)mnm
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
                            print_u8(0, backingSrc, (u8*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::U16) {
                            print_u16(0, backingSrc, (u16*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::U32) {
                            print_u32(0, backingSrc, (u32*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::U64) {
                            print_u64(0, backingSrc, (u64*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S8) {
                            print_s8(0, backingSrc, (s8*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S16) {
                            print_s16(0, backingSrc, (s16*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S32) {
                            print_s32(0, backingSrc, (s32*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::S64) {
                            print_s64(0, backingSrc, (s64*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::FLOAT) {
                            print_float(0, backingSrc, (float*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::DOUBLE) {
                            print_double(0, backingSrc, (double*)src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::TEXT) {
                            print_text(0, backingSrc, src, 1, slot.formatHex);
                        } else if (slot.dataType == HexDumpDataType::CODE_AARCH64) {
                            // code is formatted 4B per line, so 0x10 stride is split into 4 output lines
                            print_code_aarch64(0, backingSrc, src, 4, slot.formatHex);
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
