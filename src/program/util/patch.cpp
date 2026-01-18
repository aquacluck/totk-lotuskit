#include "exlaunch.hpp"
#include "util/fs.hpp"
#include "util/patch.hpp"
#include <nn/fs.h>

namespace lotuskit::util::patch {
    const std::string RUNTIME_PATCHES_DIR = "sdcard:/atmosphere/contents/0100f2c0115b6000/runtime_patches"; // XXX lowercase
    const std::string EXT_IPS = ".ips";
    const std::string EXT_RIPS = ".rips";

    #ifdef   TOTK_100
    const std::string MAIN_NSOBID = "082CE09B06E33A123CB1E2770F5F9147709033DB";
    #elifdef TOTK_110
    const std::string MAIN_NSOBID = "D5AD6AC71EF53E3E52417C1B81DBC9B4142AA3B3";
    #elifdef TOTK_121
    const std::string MAIN_NSOBID = "9B4E43650501A4D4489B4BBFDB740F26AF3CF850";
    #endif

    void PrepareRevertPatchImpl(const std::string& patchsetName) {
        const auto  ipsFilename = RUNTIME_PATCHES_DIR + "/" + patchsetName + "/" + MAIN_NSOBID + EXT_IPS;
        const auto ripsFilename = RUNTIME_PATCHES_DIR + "/" + patchsetName + "/" + MAIN_NSOBID + EXT_RIPS;
        if (!lotuskit::util::fs::fileExists(ipsFilename)) { svcOutputDebugString("no ips", 6); return; }

        // setup ips
        nn::fs::FileHandle ipsFd;
        nn::Result res = nn::fs::OpenFile(&ipsFd, ipsFilename.c_str(), nn::fs::OpenMode_Read);
        if (!res.IsSuccess()) { svcOutputDebugString("fail open ips", 13); return; }
        s64 ipsLen = 0;
        res = nn::fs::GetFileSize(&ipsLen, ipsFd);
        if (!res.IsSuccess()) { svcOutputDebugString("fail size ips", 13); return; }
        if (ipsLen < 10) { svcOutputDebugString("fail null ips", 13); return; } // IPS32EEOF smallest valid
        char magic[8];
        res = nn::fs::ReadFile(ipsFd, 0, magic, 8);
        if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); return; }
        magic[5] = '\0';
        if (strcmp(magic, "IPS32")) { svcOutputDebugString("fail not IPS32", 14); return; }

        // setup rips, identical size/recordSize/recordOffsets to ips
        nn::fs::FileHandle ripsFd;
        s64 ripsLen = 0;
        if (lotuskit::util::fs::fileExists(ripsFilename)) {
            res = nn::fs::OpenFile(&ripsFd, ripsFilename.c_str(), nn::fs::OpenMode_ReadWrite);
            if (!res.IsSuccess()) { svcOutputDebugString("fail open rips", 14); return; }

            res = nn::fs::GetFileSize(&ripsLen, ripsFd);
            if (!res.IsSuccess()) { svcOutputDebugString("fail size rips", 14); return; }

            if (ripsLen != ipsLen) {
                res = nn::fs::SetFileSize(ripsFd, ipsLen);
                if (!res.IsSuccess()) { svcOutputDebugString("fail resize rips", 16); return; }
                ripsLen = ipsLen;
            }

        } else {
            res = nn::fs::CreateFile(ripsFilename.c_str(), ipsLen);
            if (!res.IsSuccess()) { svcOutputDebugString("fail create rips", 15); return; }

            res = nn::fs::OpenFile(&ripsFd, ripsFilename.c_str(), nn::fs::OpenMode_ReadWrite);
            if (!res.IsSuccess()) { svcOutputDebugString("fail open rips", 14); return; }

            res = nn::fs::SetFileSize(ripsFd, ipsLen);
            if (!res.IsSuccess()) { svcOutputDebugString("fail resize rips", 16); return; }
            ripsLen = ipsLen;
        }

        // compare magic
        constexpr auto NOFLUSH = nn::fs::WriteOption(0);
        s64 fdOffset = 0;
        res = nn::fs::ReadFile(ripsFd, fdOffset, magic, 8);
        if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
        magic[5] = '\0';
        if (strcmp(magic, "IPS32")) {
            res = nn::fs::WriteFile(ripsFd, fdOffset, "IPS32", 5, NOFLUSH);
            if (!res.IsSuccess()) { svcOutputDebugString("fail write", 10); goto RETURN; }
        }
        fdOffset += 5;

        u8 ipsOffset[5];
        u8 ripsOffset[4];
        u8 ipsPatchSize[2];
        u8 ripsPatchSize[2];
        while (true) { // compare patch records
            // compare+write offset
            res = nn::fs::ReadFile(ipsFd, fdOffset, ipsOffset, 4);
            if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
            ipsOffset[4] = '\0';
            if (!strcmp((char*)ipsOffset, "EEOF")) { goto EEOF; } // break
            res = nn::fs::ReadFile(ripsFd, fdOffset, ripsOffset, 4);
            if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
            if (*(u32*)(ipsOffset) != *(u32*)(ripsOffset)) {
                res = nn::fs::WriteFile(ripsFd, fdOffset, ipsOffset, 4, NOFLUSH);
                if (!res.IsSuccess()) { svcOutputDebugString("fail write", 10); goto RETURN; }
            }
            fdOffset += 4;

            // compare+write patch size
            res = nn::fs::ReadFile(ipsFd, fdOffset, ipsPatchSize, 2);
            if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
            res = nn::fs::ReadFile(ripsFd, fdOffset, ripsPatchSize, 2);
            if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
            if (*(u16*)(ipsPatchSize) != *(u16*)(ripsPatchSize)) {
                res = nn::fs::WriteFile(ripsFd, fdOffset, ipsPatchSize, 2, NOFLUSH);
                if (!res.IsSuccess()) { svcOutputDebugString("fail write", 10); goto RETURN; }
            }
            fdOffset += 2;

            // compare+write revert patch payload based on current loaded code
            u16 payloadSize = ipsPatchSize[0]*0x100 + ipsPatchSize[1];
            if (payloadSize % 4 != 0) { svcOutputDebugString("fail 4B align", 13); goto RETURN; } // only patch whole instructions, simpler for exl
            while (payloadSize > 0) {
                u8 ripsInst[4];
                res = nn::fs::ReadFile(ripsFd, fdOffset, ripsInst, 4);
                if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }

                // get vanilla instruction (unless real ips or hook has altered it already)
                constexpr size_t NSO_OFFSET = 0x100;
                u8* inst = (u8*)(ipsOffset[0]*0x1000000 + ipsOffset[1]*0x10000 + ipsOffset[2]*0x100 + ipsOffset[3] - NSO_OFFSET);
                inst = reinterpret_cast<u8*>(exl::util::modules::GetTargetOffset((uintptr_t)inst));

                if (*(u32*)(inst) != *(u32*)(ripsInst)) {
                    // write patch payload to restore vanilla instruction
                    res = nn::fs::WriteFile(ripsFd, fdOffset, inst, 4, NOFLUSH);
                    if (!res.IsSuccess()) { svcOutputDebugString("fail write", 10); goto RETURN; }
                }

                payloadSize -= 4;
                fdOffset += 4;
            }
        }

        EEOF:
        u8 eeof[5];
        res = nn::fs::ReadFile(ripsFd, fdOffset, eeof, 4);
        if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
        eeof[4] = '\0';
        if (strcmp((char*)eeof, "EEOF")) {
            res = nn::fs::WriteFile(ripsFd, fdOffset, "EEOF", 4, NOFLUSH);
            if (!res.IsSuccess()) { svcOutputDebugString("fail write", 10); goto RETURN; }
        }
        fdOffset += 4; // assert fdOffset == ipsLen-1

        RETURN:
        nn::fs::FlushFile(ripsFd);
        nn::fs::CloseFile(ripsFd);
        nn::fs::CloseFile(ipsFd);
    }

    void PrepareRevertPatches() {
        if (!lotuskit::util::fs::folderExists(RUNTIME_PATCHES_DIR)) { svcOutputDebugString("no RUNTIME_PATCHES_DIR", 18); return; }
        lotuskit::util::fs::visitDirectoryEntries(RUNTIME_PATCHES_DIR, nn::fs::OpenDirectoryMode::OpenDirectoryMode_Directory, [](const std::string& cpath, nn::fs::DirectoryEntry* dentry) {
            PrepareRevertPatchImpl(dentry->mName);
        });
    }

    void ApplyPatchImpl(const std::string& ipsFilename) {
        if (!lotuskit::util::fs::fileExists(ipsFilename)) { svcOutputDebugString("no ips", 6); return; }
        s64 fdOffset = 0;
        u8 ipsOffset[5];
        u8 ipsPatchSize[2];

        // setup ips
        nn::fs::FileHandle ipsFd;
        nn::Result res = nn::fs::OpenFile(&ipsFd, ipsFilename.c_str(), nn::fs::OpenMode_Read);
        if (!res.IsSuccess()) { svcOutputDebugString("fail open ips", 13); return; }
        s64 ipsLen = 0;
        res = nn::fs::GetFileSize(&ipsLen, ipsFd);
        if (!res.IsSuccess()) { svcOutputDebugString("fail size ips", 13); goto RETURN; }
        if (ipsLen < 10) { svcOutputDebugString("fail null ips", 13); goto RETURN; } // IPS32EEOF smallest valid
        char magic[8];
        res = nn::fs::ReadFile(ipsFd, 0, magic, 8);
        if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
        magic[5] = '\0';
        if (strcmp(magic, "IPS32")) { svcOutputDebugString("fail not IPS32", 14); goto RETURN; }
        fdOffset += 5;

        while (true) { // apply patch records
            // read offset, check for eeof
            res = nn::fs::ReadFile(ipsFd, fdOffset, ipsOffset, 4);
            if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
            ipsOffset[4] = '\0';
            if (!strcmp((char*)ipsOffset, "EEOF")) { goto RETURN; } // break
            fdOffset += 4;

            // read patch size
            res = nn::fs::ReadFile(ipsFd, fdOffset, ipsPatchSize, 2);
            if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }
            fdOffset += 2;

            // apply patch payload
            u16 payloadSize = ipsPatchSize[0]*0x100 + ipsPatchSize[1];
            if (payloadSize % 4 != 0) { svcOutputDebugString("fail 4B align", 13); goto RETURN; } // only patch whole instructions, simpler for exl
            while (payloadSize > 0) {
                u32 ipsInst;
                res = nn::fs::ReadFile(ipsFd, fdOffset, &ipsInst, 4);
                if (!res.IsSuccess()) { svcOutputDebugString("fail read", 9); goto RETURN; }

                constexpr size_t NSO_OFFSET = 0x100;
                uintptr_t addr = ipsOffset[0]*0x1000000 + ipsOffset[1]*0x10000 + ipsOffset[2]*0x100 + ipsOffset[3] - NSO_OFFSET;
                exl::patch::CodePatcher(addr).WriteInst(exl::armv8::InstBitSet(ipsInst));

                payloadSize -= 4;
                fdOffset += 4;
            }
        }

        RETURN:
        nn::fs::CloseFile(ipsFd);
    }

    void PatchInstall(const std::string& patchsetName) {
        ApplyPatchImpl(RUNTIME_PATCHES_DIR + "/" + patchsetName.c_str() + "/" + MAIN_NSOBID + EXT_IPS);
    }

    void PatchRevert(const std::string& patchsetName) {
        ApplyPatchImpl(RUNTIME_PATCHES_DIR + "/" + patchsetName.c_str() + "/" + MAIN_NSOBID + EXT_RIPS);
    }

} // ns

