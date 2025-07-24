#pragma once
// exlaunch
#include <lib.hpp>

inline ptrdiff_t EXL_SYM_OFFSET(std::string sym) {
    auto search = exl::reloc::GetLookupTable().FindByName(sym);
    if (search == nullptr) { return 0; }
    return search->m_Offset;
}

template<class T>
inline T EXL_SYM_RESOLVE(std::string sym) {
    //define EXL_SYM_RESOLVE(t, sym) reinterpret_cast<t>(exl::util::modules::GetTargetStart() + EXL_SYM_OFFSET(sym));
    auto search = exl::reloc::GetLookupTable().FindByName(sym);
    if (search == nullptr) { return nullptr; }
    return reinterpret_cast<T>(exl::util::modules::GetTargetStart() + search->m_Offset); // TODO other modules
}

