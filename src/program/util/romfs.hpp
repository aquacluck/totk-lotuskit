#pragma once
#include "nn/fs.h"
#include <cstring>

namespace lotuskit::util::romfs {
    bool FileExists(const char* path) {
        nn::fs::FileHandle fd;
        nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Read);
        if (!res.IsSuccess()) {
            return false;
        }
        nn::fs::CloseFile(fd);
        return true;
    }

    void ReadTextFile(char* out, const char* path, s64 maxLen, const char* default_) {
        char errbuf[200];

        nn::fs::FileHandle fd;
        nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Read);

        if (!res.IsSuccess()) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] cant open file %s ", path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }

        s64 fdLen = 0;
        res = nn::fs::GetFileSize(&fdLen, fd);
        if (!res.IsSuccess()) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] cant get file size %s ", path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }
        if (fdLen > maxLen) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] file contents too long %d > %d %s ", fdLen, maxLen, path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }

        res = nn::fs::ReadFile(fd, 0, out, fdLen);
        if (!res.IsSuccess()) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] file read fail %s ", path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }

        nn::fs::CloseFile(fd);
    }
}
