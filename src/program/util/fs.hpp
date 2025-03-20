#pragma once
#include "exlaunch.hpp"

namespace lotuskit::util::fs {
    bool fileExists(const char* path);
    bool readTextFile(char* out, s64 maxOut, const char* path); // return true when out=error
}
