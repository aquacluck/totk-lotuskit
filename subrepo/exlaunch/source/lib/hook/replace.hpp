#pragma once

#include "base.hpp"
#include "util/func_ptrs.hpp"
#include "util/type_traits.hpp"

#define HOOK_DEFINE_REPLACE(name)                        \
struct name : public ::exl::hook::impl::ReplaceHook<name>

namespace exl::hook::impl {

    template<typename Derived>
    struct ReplaceHook {

        template<typename T = Derived>
        using CallbackFuncPtr = decltype(&T::Callback);

        static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            hook::Hook(util::modules::GetTargetStart() + address, Derived::Callback);
        }

        template<typename T>
        static ALWAYS_INLINE void InstallAtFuncPtr(T ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            using Traits = util::FuncPtrTraits<T>;
            static_assert(std::is_same_v<typename Traits::CPtr, CallbackFuncPtr<>>, "Argument pointer type must match callback type!");

            hook::Hook(ptr, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            hook::Hook(ptr, Derived::Callback);
        }

        static ALWAYS_INLINE void Install() {
            _HOOK_STATIC_CALLBACK_ASSERT();

            const auto entry = exl::reloc::GetLookupTable().FindByName(Derived::s_name);
            //auto mod = exl::util::GetModuleInfo(entry->m_ModuleIndex); // XXX cheat: always main module anyways for now
            hook::Hook(util::modules::GetTargetOffset(entry->m_Offset), Derived::Callback);
        }
    };
}
