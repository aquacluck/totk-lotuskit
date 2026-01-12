#include "util/trace.hpp"
#ifdef DEMANGLE
#include "cxxabi.h"
#endif
#include <cstring>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include "HexDump.hpp"
#include "TextWriter.hpp"

#if ENABLE_LOG_HEAP_MIN_FREE
namespace lotuskit::util::trace::HeapWatch {
    static u32 minFree[30] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                              0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                              0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

    const char* watchHeaps[30] = {"ActorInstanceHeap", "SaveLoad", "EventHeap", "Sound", "GameUIWorkHeap", "NnVfxDataHeap", "VfxDynamicHeap", "ModuleSystem",
                                  "Create Terrain Scene Heap", "Cave Module", "agl::WorkHeap", "UIHeap", "EffectModel", "BancModule", "CreateModelScene",
                                  "Chm Module Heap L", "NavMeshMgr", "ReplaceModelHeap", "StaticCompoundBodyMgr", "Sound", "aglTextureDataMgr::Unit",
                                  "Terrain Resource Heap", "GameTerrain", "DummyGeneralFormatParamMgrHeap", "Terrain Temp Heap", "Chm Module Heap S",
                                  "PlayReporter", "Challenge", "zsdic", "NinVirtualAddressHeap"};

    void heap_postalloc(sead::Heap* heap) {
        const char* name = heap->getName().cstr();
        for(int i=0; i < 30; i++) {
            if (!strcmp(name, watchHeaps[i])) {
                // XXX multiple of same name isn't correctly handled anywhere (ModuleSystem, Sound, Challenge, GameTerrain)
                auto free = heap->getFreeSize();
                if (free < minFree[i]) {
                    minFree[i] = free;
                }
                break;
            }
        }
    }

    void heap_print_min_free_impl(sead::Heap* dumproot) {
        sead::Heap* h = dumproot; // current/visiting entry
        sead::Heap* htmp = nullptr;
        u32 depth = 0;

        HEAPINFO_VISIT:
        for(int i=0; i < 30; i++) {
            if (strcmp(h->getName().cstr(), watchHeaps[i]) != 0) {
                continue;
            }
            // log the entry
            if (minFree[i] == 0xffffffff) {
                lotuskit::TextWriter::printf(0, "no_alloc %08x %08x %s(%p)\n", h->getFreeSize(), h->getSize(), watchHeaps[i], h);
            } else {
                lotuskit::TextWriter::printf(0, "%08x %08x %08x %s(%p)\n", minFree[i], h->getFreeSize(), h->getSize(), watchHeaps[i], h);
            }
            break;
        }

        // keep descending into first child
        htmp = h->mChildren.front();
        if (htmp != nullptr) {
            depth++;
            h = htmp;
            goto HEAPINFO_VISIT;
        }

        // no children, proceed to siblings
        HEAPINFO_DO_NEXT:
        if (h->mParent == nullptr) {
            // assert h == dumproot
            return; // ok
        }
        htmp = h->mParent->mChildren.next(h);
        if (htmp != nullptr) {
            h = htmp;
            goto HEAPINFO_VISIT; // advance through siblings
        }

        // no more siblings, ascend and continue advancing through the aunts and uncles
        depth--;
        h = h->mParent;
        goto HEAPINFO_DO_NEXT; // advance through siblings

        return; // unreachable
    }

    void print_min_free() {
        lotuskit::TextWriter::printf(0, "min_free     free     size name(addr):\n");
        heap_print_min_free_impl(*EXL_SYM_RESOLVE<sead::Heap**>("_ZN4sead7HeapMgr22sNinVirtualAddressHeapE"));
        heap_print_min_free_impl(sead::HeapMgr::sRootHeaps[0]);
    }

    HOOK_DEFINE_TRAMPOLINE(SeadHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadExpHeapTryAllocHook) {
        //inline static u32 count = 0;
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            /*
            if (!strcmp(self->getName().cstr(), "ModuleSystem")) { // && self->getFreeSize() > 0x10000) {
                char buf[200];
                nn::util::SNPrintf(buf, sizeof(buf), "sead::ExpHeap(%p)::alloc(%d) free=%08x count=%d", self, size, self->getFreeSize(), ++SeadExpHeapTryAllocHook::count);
                svcOutputDebugString(buf, strlen(buf));
            }
            */
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadExpHeapFreeHook) {
        //inline static u32 count = 0;
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
            /*
            if (!strcmp(self->getName().cstr(), "ModuleSystem")) { // && self->getFreeSize() > 0x10000) {
                if (++SeadExpHeapFreeHook::count >= 3575) { // target 3578,3579
                    char buf[200];
                    nn::util::SNPrintf(buf, sizeof(buf), "sead::ExpHeap(%p)::free(%p) free=%08x count=%d", self, ptr, self->getFreeSize(), ++SeadExpHeapFreeHook::count);
                    svcOutputDebugString(buf, strlen(buf));

                    //#include "util/trace.hpp"
                    lotuskit::util::trace::StackTrace trace;
                    trace.unwind();
                    trace.debugLog();

                    //lotuskit::HexDump::print_text(-1, self, self, 4);
                    //lotuskit::util::trace::StackDump::print();
                    //lotuskit::util::trace::RegDump::print(-1, ctx);
                }
            }
            */
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadFrameHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadSeparateHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadSeparateHeapFreeHook) {
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadSpareExpHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadSpareExpHeapFreeHook) {
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadSpareFrameHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadUnknown0HeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadUnknown1HeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadUnknown1HeapFreeHook) {
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadUnknown2HeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadNinStandardAllocatorHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadNinStandardAllocatorHeapFreeHook) {
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadUnitHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadUnitHeapFreeHook) {
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadVirtualAddressHeapTryAllocHook) {
        static void* Callback(sead::Heap* self, size_t size, s32 alignment) {
            auto ret = Orig(self, size, alignment);
            heap_postalloc(self);
            return ret;
        }
    };
    HOOK_DEFINE_TRAMPOLINE(SeadVirtualAddressHeapFreeHook) {
        static void Callback(sead::Heap* self, void* ptr) {
            heap_postalloc(self);
            Orig(self, ptr);
        }
    };

    void install_hooks() {
        // XXX 1.0 symbols only
        SeadHeapTryAllocHook::InstallAtOffset(0x010dc358);
        //SeadHeapFreeHook
        SeadExpHeapTryAllocHook::InstallAtOffset(0x00b21d50);
        SeadExpHeapFreeHook::InstallAtOffset(0x029b72fc);
        SeadFrameHeapTryAllocHook::InstallAtOffset(0x00b21d50);
        //SeadFrameHeapFreeHook
        SeadSeparateHeapTryAllocHook::InstallAtOffset(0x029bd888);
        SeadSeparateHeapFreeHook::InstallAtOffset(0x029bdad4);
        SeadSpareExpHeapTryAllocHook::InstallAtOffset(0x00b21ab0);
        SeadSpareExpHeapFreeHook::InstallAtOffset(0x029b83a8);
        SeadSpareFrameHeapTryAllocHook::InstallAtOffset(0x029b8940);
        //SeadSpareFrameHeapFreeHook
        SeadUnknown0HeapTryAllocHook::InstallAtOffset(0x02a5f560);
        //SeadUnknown0HeapFreeHook::InstallAtOffset(0x02a5f5ac); // empty impl (frameheap?)
        SeadUnknown1HeapTryAllocHook::InstallAtOffset(0x02a609a0);
        SeadUnknown1HeapFreeHook::InstallAtOffset(0x02a60a00);
        SeadUnknown2HeapTryAllocHook::InstallAtOffset(0x02a61998);
        //SeadUnknown2HeapFreeHook::InstallAtOffset(0x02a619e4); // empty impl (frameheap?)
        SeadNinStandardAllocatorHeapTryAllocHook::InstallAtOffset(0x02a7fd38);
        SeadNinStandardAllocatorHeapFreeHook::InstallAtOffset(0x02a7fde4);
        SeadUnitHeapTryAllocHook::InstallAtOffset(0x029b90b8);
        SeadUnitHeapFreeHook::InstallAtOffset(0x029b91d0);
        SeadVirtualAddressHeapTryAllocHook::InstallAtOffset(0x008421d4);
        SeadVirtualAddressHeapFreeHook::InstallAtOffset(0x02150920);
    }
} // namespace lotuskit::util::trace::HeapWatch
#endif // ENABLE_LOG_HEAP_MIN_FREE

namespace nn::diag::detail {
    u64 GetNearestExportedSymbol(char const**, u64*, u64);
}

namespace lotuskit::util::trace {
    class AddressInfo {
        public:
        uintptr_t addr = 0;
        const char* module_name = nullptr;
        ptrdiff_t offset = 0;
        char nearest_sym_name[0x80];
        ptrdiff_t nearest_sym_offset = 0;

        void read(uintptr_t addr) {
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

    void RegDump::print(s64 drawList_i, exl::hook::InlineCtx* ctx, u8 x0, u8 xL) {
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "InlineCtx:\n");
        if (drawList_i == -1) {
            svcOutputDebugString(buf, strlen(buf)-1); // no newline
        } else {
            lotuskit::TextWriter::printf(drawList_i, buf);
        }
        for (u8 xi = x0; xi < xL; xi++) {
            nn::util::SNPrintf(buf, sizeof(buf), "X[%02d]: %p\n", xi, ctx->X[xi]);
            if (drawList_i == -1) {
                svcOutputDebugString(buf, strlen(buf)-1); // no newline
            } else {
                lotuskit::TextWriter::printf(drawList_i, buf);
            }
        }
    }

    void StackDump::print(s64 drawList_i, size_t linecount) {
        // TODO add "sp+{offset}" hexdump leader format? limit to stack bounds?
        auto sp = (void*)exl::util::stack_trace::GetSp();
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "stack dump (sp=%p):\n", sp);
        if (drawList_i == -1) {
            svcOutputDebugString(buf, strlen(buf)-1); // no newline
        } else {
            lotuskit::TextWriter::printf(drawList_i, buf);
        }
        lotuskit::HexDump::print_text(drawList_i, sp, sp, linecount);
    }

    // thanks dt-13269 for initial StackTrace impl
    // this is essentially the atmosphere implementation
    // https://github.com/Atmosphere-NX/Atmosphere/blob/master/stratosphere/creport/source/creport_threads.cpp#L37
    void StackTrace::unwind() {
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

    void StackTrace::print(s64 drawList_i) {
        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "stack trace:\n");
        if (drawList_i == -1) {
            svcOutputDebugString(buf, strlen(buf)-1); // no newline
        } else {
            lotuskit::TextWriter::printf(drawList_i, buf);
        }
        AddressInfo info;
        for (size_t i = 0; i < trace_size; ++i) {
            info.read(trace[i]);
            if (info.module_name) {
                if (info.nearest_sym_name[0] == '\0') {
                    nn::util::SNPrintf(buf, sizeof(buf), "%p => %s+%08x\n", info.addr, info.module_name, info.offset);
                } else {
                    nn::util::SNPrintf(buf, sizeof(buf), "%p => %s+%08x [%s+%08x]\n", info.addr, info.module_name, info.offset, info.nearest_sym_name, info.nearest_sym_offset);
                }
            } else {
                nn::util::SNPrintf(buf, sizeof(buf), "%p\n", info.addr);
            }
            if (drawList_i == -1) {
                svcOutputDebugString(buf, strlen(buf)-1); // no newline
            } else {
                lotuskit::TextWriter::printf(drawList_i, buf);
            }
        }
    }

    /*
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
    */

} // namespace

