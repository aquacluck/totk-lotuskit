#pragma once

#include "types.h"
#include <type_traits>
#include <cstdarg>

namespace nn {
    namespace util {
        template<s32 size, typename T>
        struct BitFlagSet {
            using type = std::conditional_t<size <= 32, u32, u64>;
            static const int storageBits = static_cast<int>(sizeof(type)) * 8;
            static const int storageCount = static_cast<int>((size + storageBits - 1)) / storageBits;
            type field[storageCount];

            inline bool isBitSet(T index) const {
                return (this->field[static_cast<u64>(index) / storageBits] &
                        (static_cast<type>(1) << static_cast<u64>(index) % storageBits)) != 0;
            }
        };

        // XXX i've seen both s32 u32 used here idk
        u32 SNPrintf(char *output, size_t max_size, const char *format, ...);
        u32 VSNPrintf(char *output, size_t max_size, const char *format, va_list args);
    }
}
