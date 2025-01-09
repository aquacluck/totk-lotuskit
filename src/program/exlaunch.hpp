#pragma once
#include <string>

// exlaunch
#include <lib.hpp>

inline ptrdiff_t EXL_SYM_OFFSET(std::string sym) {
    return exl::reloc::GetLookupTable().FindByName(sym)->m_Offset;
}

template<class T>
inline T EXL_SYM_RESOLVE(std::string sym) {
    //define EXL_SYM_RESOLVE(t, sym) reinterpret_cast<t>(exl::util::modules::GetTargetStart() + EXL_SYM_OFFSET(sym));
    return reinterpret_cast<T>(exl::util::modules::GetTargetStart() + EXL_SYM_OFFSET(sym)); // TODO other modules
}

