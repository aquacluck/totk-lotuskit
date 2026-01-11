#pragma once
#include "lib.hpp"
#ifdef DEMANGLE
#include "cxxabi.h"
#endif
#include <cstring>
#include "HexDump.hpp"
// thanks dt-13269 for putting together initial impl

namespace nn::diag::detail {
    u64 GetNearestExportedSymbol(char const**, u64*, u64);
}

namespace lotuskit::util::trace {
    /*
    inline void demangle(char* mangled_name, char* out_buffer, size_t out_size) {
        #ifdef DEMANGLE
        int status;
        char* demangled = abi::__cxa_demangle(mangled_name, nullptr, 0, &status);
        if (status) {
            size_t copy_size = strnlen(mangled_name, out_size);
            memcpy(out_buffer, mangled_name, copy_size);
            out_buffer[copy_size] = '\0';
        } else {
            size_t copy_size = strnlen(demangled, out_size);
            memcpy(out_buffer, demangled, copy_size);
            out_buffer[copy_size] = '\0';
        }
        free(demangled);
        #else
        size_t copy_size = strnlen(mangled_name, out_size);
        memcpy(out_buffer, mangled_name, copy_size);
        out_buffer[copy_size] = '\0';
        #endif
    }
    */

    class AddressInfo {
        public:
        uintptr_t addr = 0;
        const char* module_name = nullptr;
        ptrdiff_t offset = 0;
        char nearest_sym_name[0x80];
        ptrdiff_t nearest_sym_offset = 0;

        inline void read(uintptr_t addr) {
            this->addr = addr;
            auto module = exl::util::TryGetModule(addr);
            if (module == nullptr) {
                module_name = nullptr;
                return;
            }
            offset = addr - module->m_Total.m_Start;
            module_name = module->GetModuleName().data();

            u64 sym_size;
            char const* sym_name;
            uintptr_t sym_addr = nn::diag::detail::GetNearestExportedSymbol(&sym_name, &sym_size, addr);
            if (sym_addr == 0 || addr - sym_addr >= sym_size) {
                nearest_sym_name[0] = '\0';
                nearest_sym_offset = 0;
            } else {
                nearest_sym_offset = addr - sym_addr;
                size_t copy_size = strnlen(sym_name, sizeof(nearest_sym_name));
                memcpy(nearest_sym_name, sym_name, copy_size);
                nearest_sym_name[copy_size] = '\0';
            }
        }
    };

    template <size_t max_trace_size = 0x20>
    struct StackTrace {
        StackTrace() = default;
        size_t trace_size = 0;
        uintptr_t trace[max_trace_size];

        // this is essentially the atmosphere implementation
        // https://github.com/Atmosphere-NX/Atmosphere/blob/master/stratosphere/creport/source/creport_threads.cpp#L37
        inline void unwind() {
            auto* current_frame = exl::util::stack_trace::GetCurrentFrame();
            uintptr_t current_fp = current_frame->m_Fp;
            trace_size = 0;
            for (size_t i = 0; i < max_trace_size; ++i) {
                if (current_fp == 0 || current_fp % sizeof(void*) != 0) {
                    break;
                }
                current_frame = reinterpret_cast<exl::util::stack_trace::Frame*>(current_fp);
                if (current_frame == 0 || current_frame->m_Lr == 0 || current_frame->m_Lr % 4 != 0) {
                    break;
                }
                trace[trace_size++] = current_frame->m_Lr;
                current_fp = current_frame->m_Fp;
            }
        }

        inline void debugLog() {
            char buf[200];
            AddressInfo info;
            for (size_t i = 0; i < trace_size; ++i) {
                info.read(trace[i]);
                if (info.module_name) {
                    if (info.nearest_sym_name[0] == '\0') {
                        nn::util::SNPrintf(buf, sizeof(buf), "%p => %s+%08x", info.addr, info.module_name, info.offset);
                    } else {
                        nn::util::SNPrintf(buf, sizeof(buf), "%p => %s+%08x [%s+%08x]", info.addr, info.module_name, info.offset, info.nearest_sym_name, info.nearest_sym_offset);
                    }
                } else {
                    nn::util::SNPrintf(buf, sizeof(buf), "%p", info.addr);
                }
                svcOutputDebugString(buf, strlen(buf));
            }

            auto sp = (void*)exl::util::stack_trace::GetSp();
            nn::util::SNPrintf(buf, sizeof(buf), "stack dump (sp=%p):", sp);
            svcOutputDebugString(buf, strlen(buf));
            lotuskit::HexDump::print_text(-1, sp-0x40, sp-0x40, 8); // +-4 lines
        }
    };

} // namespace

