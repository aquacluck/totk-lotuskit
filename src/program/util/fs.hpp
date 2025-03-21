#pragma once
#include "exlaunch.hpp"
#include <nn/fs.h>

namespace lotuskit::util::fs {
    bool fileExists(const char* path);
    bool readTextFile(char* out, s64 maxOut, const char* path); // return true when out=error
    bool writeTextFile(const char* src, const char* path); // return true when error
    bool writeFile(const void* src, size_t srcLen, const char* path); // return true when error

    // write a file split across several calls -- gross hack
    void writeFileChunked(nn::fs::FileHandle* workingFd, const void* src, size_t srcLen, const char* path, u32 chunkOp, u64 chunkOffset);
}
