#pragma once
#include <string>
#include "structs/engineSystemPause.hpp"
#include "exlaunch.hpp"

namespace lotuskit::util::hash {

    inline u32 murmur32(const String& v) {
        using impl_t = u32 (const void*, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("engine::util::HashMurmur32");
        return impl(v.c_str(), 0);
    }

} // ns

