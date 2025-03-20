#include "util/fs.hpp"
#include "exlaunch.hpp"
#include "nn/fs.h"
#include <cstring>

namespace lotuskit::util::fs {
    bool fileExists(const char* path) {
        nn::fs::FileHandle fd;
        nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Read);
        if (!res.IsSuccess()) {
            return false;
        }
        nn::fs::CloseFile(fd);
        return true;
    }

    bool readTextFile(char* out, s64 maxOut, const char* path) {
        char errbuf[200]; // assert maxOut > 200
        nn::fs::FileHandle fd;
        nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Read);

        if (!res.IsSuccess()) {
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "cant open file %s ", path);
            strcpy(out, errbuf);
            return true; // err
        }

        s64 fdLen = 0;
        res = nn::fs::GetFileSize(&fdLen, fd);
        if (!res.IsSuccess()) {
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "cant get file size %s ", path);
            strcpy(out, errbuf);
            return true; // err
        }
        if (fdLen > maxOut) {
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "file contents too long %d > %d %s ", fdLen, maxOut, path);
            strcpy(out, errbuf);
            return true; // err
        }

        res = nn::fs::ReadFile(fd, 0, out, fdLen);
        if (!res.IsSuccess()) {
            nn::util::SNPrintf(errbuf, sizeof(errbuf), "file read fail %s ", path);
            strcpy(out, errbuf);
            return true; // err
        }

        nn::fs::CloseFile(fd);
        return false; // ok
    }

    bool writeTextFile(const char* src, const char* path) {
        return writeFile(src, strlen(src), path);
    }

    bool writeFile(const void* src, size_t srcLen, const char* path) {
        // ty Watertoon for mc_decompressor as reference (GPL2, https://gamebanana.com/tools/13236)
        nn::fs::FileHandle fd;
        nn::fs::CreateDirectory("sdcard:/totk_lotuskit"); // ensure folder present, ignore result

        nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Write);
        if (!res.IsSuccess()) { // assert !fileExists(path), create output file
            res = nn::fs::CreateFile(path, srcLen);
            if (!res.IsSuccess()) { return true; } // err: failed to create
            res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Write);
        }
        if (!res.IsSuccess()) { return true; } // err: failed to open

        res = nn::fs::SetFileSize(fd, srcLen);
        if (!res.IsSuccess()) { return true; } // err: failed to resize

        constexpr auto FLUSH = nn::fs::WriteOption(nn::fs::WriteOptionFlag_Flush);
        res = nn::fs::WriteFile(fd, 0, src, srcLen, FLUSH);
        if (!res.IsSuccess()) { return true; } // err: failed to write

        nn::fs::CloseFile(fd); // void
        return false; // ok
    }

}
