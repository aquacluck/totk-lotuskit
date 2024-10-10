#pragma once

#include "types.h"

#define EXL_MODULE_NAME "actor"
#define EXL_MODULE_NAME_LEN 8
#define EXL_DEBUG
#define EXL_USE_FAKEHEAP
//#define EXL_SUPPORTS_REBOOTPAYLOAD
#define EXL_LOAD_KIND_KIP       0
#define EXL_LOAD_KIND_ASRTLD    1
#define EXL_LOAD_KIND_MODULE    2

namespace exl::setting {
    constexpr size_t HeapSize = 0x5000; // How large the fake .bss heap will be.
    constexpr size_t JitSize = 0x1000; // How large the JIT area will be for hooks.
    constexpr size_t InlinePoolSize = 0x1000; // How large the area will be inline hook pool.
    static_assert(ALIGN_UP(JitSize, PAGE_SIZE) == JitSize, "");
    static_assert(ALIGN_UP(InlinePoolSize, PAGE_SIZE) == JitSize, "");

    enum class LoadKind {
        Kip,
        AsRtld,
        Module,
    };
    /* Ensure consistency with preprocessor constants. */
    static_assert(EXL_LOAD_KIND_KIP     == static_cast<int>(LoadKind::Kip), "");
    static_assert(EXL_LOAD_KIND_ASRTLD  == static_cast<int>(LoadKind::AsRtld), "");
    static_assert(EXL_LOAD_KIND_MODULE  == static_cast<int>(LoadKind::Module), "");

    static constexpr LoadKind SelfLoadKind = LoadKind::EXL_LOAD_KIND;
    static constexpr u64 ProgramId = EXL_PROGRAM_ID;
};

#ifndef EXL_LOAD_KIND_ENUM
#error "EXL_LOAD_KIND_ENUM not defined!"
#elif EXL_LOAD_KIND_ENUM == EXL_LOAD_KIND_KIP
    #define EXL_AS_KIP
#elif EXL_LOAD_KIND_ENUM == EXL_LOAD_KIND_ASRTLD
    #define EXL_AS_RTLD
#elif EXL_LOAD_KIND_ENUM == EXL_LOAD_KIND_MODULE
    #define EXL_AS_MODULE
#endif
