#pragma once
#include "lib.hpp"

namespace lotuskit::util::romfs {
    bool FileExists(const char* path);
    void ReadTextFile(char* out, const char* path, s64 maxLen, const char* default_);
}
