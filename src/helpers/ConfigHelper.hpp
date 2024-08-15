#pragma once

#include "nn/fs.hpp"

class ConfigHelper {
    public:
    static void ReadFile(char* out, const char* path, int max_len, const char* default_) {
        char errbuf[200];
        nn::fs::FileHandle fd;
        nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Read);
        if (!res.isSuccess()) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] cant open file %s ", path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }

        long int fd_len = 0;
        res = nn::fs::GetFileSize(&fd_len, fd);
        if (!res.isSuccess()) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] cant get file size %s ", path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }
        if (fd_len > max_len) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] file contents too long %d > %d %s ", fd_len, max_len, path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }

        res = nn::fs::ReadFile(fd, 0, out, fd_len);
        if (!res.isSuccess()) {
            strcpy(out, default_);
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] file read fail %s ", path);
            svcOutputDebugString(errbuf, strlen(errbuf));
            return;
        }

        nn::fs::CloseFile(fd);
    }

    static bool ReadFileFlag(const char* path, bool default_) {
        constexpr auto N = 2;
        char buf[N];
        ReadFile(buf, path, N, default_ ? "1" : "0");
        return buf[0] == '1';
    }
};
