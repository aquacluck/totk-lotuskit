#pragma once

#include <common.hpp>

#include "base.hpp"

#define HOOK_DEFINE_INLINE(name)                        \
struct name : public ::exl::hook::impl::InlineHook<name>

namespace exl::hook::impl {

    template<typename Derived>
    struct InlineHook {
        
        template<typename T = Derived>
        using CallbackFuncPtr = decltype(&T::Callback);

        static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            hook::HookInline(util::modules::GetTargetStart() + address, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            hook::HookInline(ptr, Derived::Callback);
        }

        static ALWAYS_INLINE void Install() {
            _HOOK_STATIC_CALLBACK_ASSERT();

            const auto entry = exl::reloc::GetLookupTable().FindByName(Derived::s_name);
            //auto mod = exl::util::GetModuleInfo(entry->m_ModuleIndex); // XXX cheat: always main module anyways for now
            hook::HookInline(util::modules::GetTargetOffset(entry->m_Offset), Derived::Callback);
        }

    };
}
