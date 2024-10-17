#pragma once
#include "lib.hpp"

namespace lotuskit::util::romfs {
    bool fileExists(const char* path);
    void readTextFile(char* out, const char* path, s64 maxLen, const char* default_);
}
