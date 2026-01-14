#!/usr/bin/env python3
import os, os.path, sys, pathlib
import struct

class BuildId:
    # main nsobid https://switchbrew.org/wiki/NSO0#ModuleId
    # $ hexdump -Cn96 main | grep '^000000[45]0'
    # 00000040  08 2c e0 9b 06 e3 3a 12  3c b1 e2 77 0f 5f 91 47  |.,....:.<..w._.G|
    # 00000050  70 90 33 db 00 00 00 00  00 00 00 00 00 00 00 00  |p.3.............|
    TOTK_100 = "082CE09B06E33A123CB1E2770F5F9147709033DB"
    TOTK_110 = "D5AD6AC71EF53E3E52417C1B81DBC9B4142AA3B3"
    TOTK_121 = "9B4E43650501A4D4489B4BBFDB740F26AF3CF850"

# IPS32 filename format is an uppercase nsobid string with ips extension. Contents:
# 5B "IPS32" magic
# patch_records[]:
#     4B NSO offset (+0x100 above ghidra .text/MOD0)
#     2B patch_size
#     {patch_size}B patch_value
# 4B "EEOF"

# input types:
PatchRecords = dict[int, bytes] # individual patches
PatchSet = dict[BuildId, PatchRecords] # group by version/target
PatchSetCollection = dict[str, PatchSet] # group by ips folder name

def write_ips(filename: str, patches: PatchRecords, offset_shift=0x100):
    # IPSwitch at home: write_ips(f"patch_name/9B4E43650501A4D4489B4BBFDB740F26AF3CF850.ips", {0x01155ec4: b"\x04\x64\xa8\xd2"})
    fd = open(filename, "wb")
    fd.write(b"IPS32")
    for offset, value in patches.items():
        fd.write(struct.pack(">I", offset+offset_shift))
        assert len(value) <= 0xffff, "patch record payload must be <64KiB"
        fd.write(struct.pack(">H", len(value)))
        fd.write(bytes(value))
    fd.write(b"EEOF")
    fd.close()

def write_ips_patchsets(patchsets: PatchSetCollection):
    for folder_name, target_builds in patchsets.items():
        pathlib.Path(folder_name).mkdir(parents=True, exist_ok=True)
        for nsobid, patch_records in target_builds.items():
            ips_filename = os.path.join(folder_name, str(nsobid).upper() + ".ips") # NSOBID.ips (case sensitive)
            write_ips(ips_filename, patch_records)

def b(v) -> bytes: # ez input
    if type(v) is bytes:
        return v
    if type(v) is int:
        v: str = hex(v)
    v = str(v).removeprefix("0x").replace(" ", "").replace("\t", "").replace("\r", "").replace("\n", "")
    if len(v) % 2:
        v = "0" + v # fromhex wants full bytes
    return bytes.fromhex(v)

LOTUSKIT_IPS_PATCHES: PatchSetCollection = {
    "lotuskit-standardallocator-shrink-2MB": {
        BuildId.TOTK_100: {
            # StandardAllocator is 9MB -> 7MB
            0x01130f38: b("00 0e a0 52"), # mov w0,#0x700000          // -2MB to engine::system::sStandardAllocatorMemBlockSize
            0x01130f64: b("01 0e a0 d2"), # mov x1,#0x700000          // -2MB to nn::init::InitializeAllocator
            0x01130f74: b("08 01 5c 91"), # add x8,x8,#0x700, LSL #12 // -2MB shift arena start
        },
        BuildId.TOTK_110: {
            # StandardAllocator is 11MB -> 9MB (untested)
            0x01160518: b("00 12 a0 52"), # mov w0,#0x900000
            0x01160544: b("01 12 a0 d2"), # mov x1,#0x900000
            0x01160554: b("08 01 64 91"), # add x8,x8,#0x900, LSL #12
        },
        BuildId.TOTK_121: {
            # StandardAllocator is 11MB -> 9MB (untested)
            0x01155eb8: b("00 12 a0 52"), # mov w0,#0x900000
            0x01155ee4: b("01 12 a0 d2"), # mov x1,#0x900000
            0x01155ef4: b("08 01 64 91"), # add x8,x8,#0x900, LSL #12
        },
    },
}

def main():
    os.chdir(os.path.dirname(__file__))
    write_ips_patchsets(LOTUSKIT_IPS_PATCHES)
    if "install" in sys.argv:
        os.system("rsync -r lotuskit-* ~/appdata/Roaming/Ryujinx/sdcard/atmosphere/exefs_patches")

if __name__ == "__main__":
    main()

