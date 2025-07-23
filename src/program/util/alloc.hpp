#pragma once
#include "exlaunch.hpp"
#include <heap/seadHeap.h>
#include <lib/json.hpp>

namespace lotuskit::util::alloc {
    inline sead::Heap* vheap = nullptr;

    // ty https://en.cppreference.com/w/cpp/named_req/Allocator
    template<class T>
    struct Mallocator {
        typedef T value_type;

        Mallocator() = default;

        template<class U>
        inline constexpr Mallocator(const Mallocator <U>&) noexcept { }

        [[nodiscard]] inline T* allocate(std::size_t n) {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
                svcOutputDebugString("ERR alloc bad length", 20);
                return nullptr; //throw std::bad_array_new_length();
            }

            if (auto p = static_cast<T*>(vheap->alloc(n * sizeof(T)))) {
                //svcOutputDebugString("myalloc", 7);
                return p;
            }

            svcOutputDebugString("ERR alloc bad       ", 20);
            return nullptr; // throw std::bad_alloc();
        }

        inline void deallocate(T* p, std::size_t n) noexcept {
            //svcOutputDebugString("myfree", 6);
            vheap->free(p);
        }
    };

    template<class T, class U>
    inline bool operator==(const Mallocator <T>&, const Mallocator <U>&) { return true; }

    template<class T, class U>
    inline bool operator!=(const Mallocator <T>&, const Mallocator <U>&) { return false; }

} // ns

namespace lotuskit {
    using String = std::basic_string<char, std::char_traits<char>, util::alloc::Mallocator<char>>;
    using json = nlohmann::basic_json<std::map, std::vector, String, bool, s64, u64, double, util::alloc::Mallocator, nlohmann::adl_serializer, std::vector<u8>>;
}

