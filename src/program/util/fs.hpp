#pragma once
#include "exlaunch.hpp"
#include <nn/fs.h>
#include <string>

namespace lotuskit::util::fs {
    bool fileExists(const std::string& path);
    bool readTextFile(char* out, s64 maxOut, const std::string& path); // return true when out=error
    bool writeTextFile(const char* src, const std::string& path); // return true when error
    bool writeFile(const void* src, size_t srcLen, const std::string& path); // return true when error
    std::string canonicalize(const std::string& path);

    // write a file split across several calls -- gross hack
    void writeFileChunked(nn::fs::FileHandle* workingFd, const void* src, size_t srcLen, const std::string& path, u32 chunkOp, u64 chunkOffset);

    using VisitCallback = void(const std::string&, nn::fs::DirectoryEntry*); // parent path, child dentry
    void visitDirectoryEntries(const std::string& path, s32 openMode, VisitCallback* cb);
}
