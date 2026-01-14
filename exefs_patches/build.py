#!/usr/bin/env python3
import os, os.path, pathlib
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
#     4B NSO offset (+0x100 from ghidra .text/MOD0)
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
    v = str(v).lstrip("0x").replace(" ", "").replace("\t", "").replace("\r", "").replace("\n", "")
    if len(v) % 2:
        v = "0" + v # fromhex wants full bytes
    return bytes.fromhex(v)

TOTK_IPS_PATCHSETS: PatchSetCollection = {
    "nn-standardallocator-shrink-2MB": {
        BuildId.TOTK_100: {
            0x01130f28: b("02 6c a8 52"), # w2,#0x43600000 # vanilla - 2MB
            0x01130f2c: b("13 0e a0 52"), # w19,#0x700000  # vanilla - 2MB
            0x01130f30: b("05 64 ae d2"), # x5,#0x73200000
            #0x01130f44: b"\x04\x6c\xa8\xd2", # mov x4,#0x43600000
        },
        BuildId.TOTK_110: {
            0x01160524: b"\x04\x64\xa8\xd2", # mov x4,#0x43200000
        },
        BuildId.TOTK_121: {
            0x01155ec4: b"\x04\x64\xa8\xd2", # mov x4,#0x43200000
        },
    },
}

def main():
    os.chdir(os.path.dirname(__file__))
    write_ips_patchsets(TOTK_IPS_PATCHSETS)

if __name__ == "__main__":
    main()

