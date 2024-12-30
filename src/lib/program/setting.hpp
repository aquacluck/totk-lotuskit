#pragma once

#include <common.hpp>

#define EXL_MODULE_NAME "lotuskit"
#define EXL_DEBUG
#define EXL_USE_FAKEHEAP
//#define EXL_SUPPORTS_REBOOTPAYLOAD

namespace exl::setting {
    constexpr size_t HeapSize = 0x10000; // How large the fake .bss heap will be.
    constexpr size_t JitSize = 0x5000; // How large the JIT area will be for hooks. Current max: 102 trampoline hooks
    constexpr size_t InlinePoolSize = 0x2000; // How large the area will be inline hook pool. Current max: 341 inline hooks
    constexpr size_t LogBufferSize = 512; // How large the formatting buffer should be for logging. The buffer will be on the stack
    static_assert(ALIGN_UP(JitSize, PAGE_SIZE) == JitSize, "");
    static_assert(ALIGN_UP(InlinePoolSize, PAGE_SIZE) == InlinePoolSize, "");
}

