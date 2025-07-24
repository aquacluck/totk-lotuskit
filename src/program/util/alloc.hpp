#pragma once
#include "exlaunch.hpp"
#include <heap/seadHeap.h>
#include <heap/seadExpHeap.h>
#include <lib/json.hpp>

namespace lotuskit::util::alloc {
    inline sead::Heap* vaheap = nullptr;
    inline sead::ExpHeap* objpool = nullptr;

    inline void init() {
        // assert only called once
        vaheap = *EXL_SYM_RESOLVE<sead::Heap**>("_ZN4sead7HeapMgr22sNinVirtualAddressHeapE");

        using impl_t = sead::ExpHeap* (size_t, const sead::SafeString&, sead::Heap*, s32, sead::Heap::HeapDirection, bool);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("sead::ExpHeap::create");
        objpool = impl(0x4000, "lotuskit::objpool", vaheap, 8, (sead::Heap::HeapDirection)1, false); // 1 = forward, 8 = alignment XXX what is arg 6? doThreadsafeImpl?
    }

    // ty https://en.cppreference.com/w/cpp/named_req/Allocator
    template<class T>
    struct VAHeapAllocator {
        typedef T value_type;
        VAHeapAllocator() = default;
        template<class U> inline constexpr VAHeapAllocator(const VAHeapAllocator <U>&) noexcept { }

        [[nodiscard]] inline T* allocate(size_t n) {
            if (n > std::numeric_limits<size_t>::max() / sizeof(T)) {
                svcOutputDebugString("ERR alloc bad length", 20);
                return nullptr;
            }
            if (auto p = static_cast<T*>(vaheap->alloc(n * sizeof(T)))) {
                return p; // ok
            }
            svcOutputDebugString("ERR alloc bad       ", 20);
            return nullptr;
        }

        inline void deallocate(T* p, std::size_t n) noexcept {
            vaheap->free(p);
        }
    };
    template<class T, class U>
    inline bool operator==(const VAHeapAllocator <T>&, const VAHeapAllocator <U>&) { return true; }
    template<class T, class U>
    inline bool operator!=(const VAHeapAllocator <T>&, const VAHeapAllocator <U>&) { return false; }

    // same as VAHeapAllocator but from objpool // XXX copypasta, idk olympic level C++ wizardposting required to make it generic (another template layer+classvar with a static heap accessor method?)
    template<class T>
    struct ObjPoolAllocator {
        typedef T value_type;
        ObjPoolAllocator() = default;
        template<class U> inline constexpr ObjPoolAllocator(const ObjPoolAllocator <U>&) noexcept { }

        [[nodiscard]] inline T* allocate(size_t n) {
            if (n > std::numeric_limits<size_t>::max() / sizeof(T)) {
                svcOutputDebugString("ERR alloc bad length", 20);
                return nullptr;
            }
            if (auto p = static_cast<T*>(objpool->alloc(n * sizeof(T)))) {
                return p; // ok
            }
            svcOutputDebugString("ERR alloc bad       ", 20);
            return nullptr;
        }

        inline void deallocate(T* p, std::size_t n) noexcept {
            //svcOutputDebugString("myfree", 6);
            objpool->free(p);
        }
    };
    template<class T, class U>
    inline bool operator==(const ObjPoolAllocator <T>&, const ObjPoolAllocator <U>&) { return true; }
    template<class T, class U>
    inline bool operator!=(const ObjPoolAllocator <T>&, const ObjPoolAllocator <U>&) { return false; }

} // ns

namespace lotuskit {
    // for file contents etc, 4KB vaheap granularity
    using BigString = std::basic_string<char, std::char_traits<char>, util::alloc::VAHeapAllocator<char>>;
    using BigBlob = std::vector<u8, util::alloc::VAHeapAllocator<u8>>;

    // alloc from sead::ExpHeap objpool for general use
    using String = std::basic_string<char, std::char_traits<char>, util::alloc::ObjPoolAllocator<char>>;
    using json = nlohmann::basic_json<std::map, std::vector, String, bool, s64, u64, double, util::alloc::ObjPoolAllocator, nlohmann::adl_serializer, std::vector<u8>>;
    using Blob = std::vector<u8, util::alloc::ObjPoolAllocator<u8>>;
}

