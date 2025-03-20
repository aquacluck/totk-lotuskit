#pragma once
#include "exlaunch.hpp"

namespace lotuskit::util::fs {
    bool fileExists(const char* path);
    bool readTextFile(char* out, s64 maxOut, const char* path); // return true when out=error
    bool writeTextFile(const char* src, const char* path); // return true when error
    bool writeFile(const void* src, size_t srcLen, const char* path); // return true when error
}
