#pragma once
#include "exlaunch.hpp"

namespace lotuskit::util::disasm {
    extern size_t MNM_OUT_MAX_SIZE;
    void u32_to_str_mnemonic(u32 inst, char* output);

} // ns

