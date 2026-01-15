#pragma once
#include "exlaunch.hpp"
#define ENABLE_LOG_HEAP_MIN_FREE (false && TOTK_VERSION == 100)

namespace lotuskit::util::trace {
    namespace HeapWatch {
        #if ENABLE_LOG_HEAP_MIN_FREE
            void InstallHooks();
            void PrintMinFree(); // TODO drawlist=-1/debug output
        #else
            inline void InstallHooks() { } // nop
            inline void PrintMinFree() { } // nop
        #endif
    }

    class RegDump {
        public:
        static void print(s64 drawList_i=-1, exl::hook::InlineCtx* ctx=0, u8 x0=0, u8 xL=31);
        //static void print(s64 drawList_i=-1, exl::hook::InlineFloatCtx* ctx=0); // TODO
    };

    class StackDump {
        public:
        static void print(s64 drawList_i=-1, size_t linecount=8);
    };

    class StackTrace {
        public: // thanks dt-13269 for initial StackTrace impl
        StackTrace() = default;
        size_t trace_size = 0;
        static constexpr size_t max_trace_size = 0x20;
        uintptr_t trace[max_trace_size];

        void unwind();
        void print(s64 drawList_i=-1);
    };

} // namespace

