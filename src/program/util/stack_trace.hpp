#pragma once
#include "lib.hpp"
#ifdef DEMANGLE
#include "cxxabi.h"
#endif
#include <cstring>
// thanks dt-13269 for putting together initial impl

namespace nn::diag::detail {
    u64 GetNearestExportedSymbol(char const**, u64*, u64);
}

namespace lotuskit::util::trace {
    struct StackFrame {
        uintptr_t fp;
        uintptr_t lr;
    };

    struct AddressInfo {
        const char* module_name;
        ptrdiff_t offset = 0;
        char nearest_sym_name[0x80];
        ptrdiff_t nearest_sym_offset = 0;
        //int module_index = -1;
    };

    void demangle(char* mangled_name, char* out_buffer, size_t out_size) {
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

    void getAddressInfo(uintptr_t addr, AddressInfo& info) {
        auto module = exl::util::TryGetModule(addr);
        if (module == nullptr) {
            info.module_name = nullptr;
            return;
        }
        info.offset = addr - module->m_Total.m_Start;
        info.module_name = module->GetModuleName().data();
        //info.module_index = i;

        u64 sym_size;
        char const* sym_name;
        uintptr_t sym_addr = nn::diag::detail::GetNearestExportedSymbol(&sym_name, &sym_size, addr);
        if (sym_addr == 0 || addr - sym_addr >= sym_size) {
            info.nearest_sym_name[0] = '\0';
            info.nearest_sym_offset = 0;
        } else {
            info.nearest_sym_offset = addr - sym_addr;
            size_t copy_size = strnlen(sym_name, sizeof(info.nearest_sym_name));
            memcpy(info.nearest_sym_name, sym_name, copy_size);
            info.nearest_sym_name[copy_size] = '\0';
        }
    }

    template <size_t max_trace_size = 0x20>
    struct StackTrace {
        StackTrace() = default;
        size_t trace_size = 0;
        uintptr_t trace[max_trace_size];
        uintptr_t getFramePtr() const { return reinterpret_cast<uintptr_t>(__builtin_frame_address(0)); }

        // this is essentially the atmosphere implementation
        // https://github.com/Atmosphere-NX/Atmosphere/blob/master/stratosphere/creport/source/creport_threads.cpp#L37
        void unwind() {
            uintptr_t current_fp = getFramePtr();
            trace_size = 0;
            for (size_t i = 0; i < max_trace_size; ++i) {
                if (current_fp == 0 || current_fp % sizeof(void*) != 0) {
                    break;
                }

                const StackFrame* current_frame = reinterpret_cast<const StackFrame*>(current_fp);
                if (current_frame == 0 || current_frame->lr == 0 || current_frame->lr % 4 != 0) {
                    break;
                }

                trace[trace_size++] = current_frame->lr;
                current_fp = current_frame->fp;
            }
        }

        void debugLog() {
            char buf[200];
            AddressInfo info;
            for (size_t i = 0; i < trace_size; ++i) {
                uintptr_t addr = trace[i];
                getAddressInfo(addr, info);
                if (info.module_name) {
                    if (info.nearest_sym_name[0] == '\0') {
                        nn::util::SNPrintf(buf, sizeof(buf), "%p => %s+%08x", addr, info.module_name, info.offset);
                    } else {
                        nn::util::SNPrintf(buf, sizeof(buf), "%p => %s+%08x [%s+%08x]", addr, info.module_name, info.offset, info.nearest_sym_name, info.nearest_sym_offset);
                    }
                } else {
                    nn::util::SNPrintf(buf, sizeof(buf), "%p", addr);
                }
                svcOutputDebugString(buf, strlen(buf));
            }
        }

    };

} // namespace

