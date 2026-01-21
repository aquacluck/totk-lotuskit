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

NOP = b("1f 20 03 d5")

# patches for installation at bootup:
LOTUSKIT_EXEFS_PATCHES: PatchSetCollection = {
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
            # StandardAllocator is 11MB -> 9MB
            0x01155eb8: b("00 12 a0 52"), # mov w0,#0x900000
            0x01155ee4: b("01 12 a0 d2"), # mov x1,#0x900000
            0x01155ef4: b("08 01 64 91"), # add x8,x8,#0x900, LSL #12
        },
    },
}

# patches for runtime installation (often large or experimental):
LOTUSKIT_RUNTIME_PATCHES: PatchSetCollection = {
    "lotuskit-playercamera-unlock": {
        BuildId.TOTK_100: {
            # 48 bf a8 52  mov w8,#0x45fa0000 (8000) ->  08 f0 af 52   mov w8,#0x7f800000 (infinity)
            0x007dd8f4: b("08 f0 af 52"),
            0x007dd910: b("08 f0 af 52"),
            0x007dd92c: b("08 f0 af 52"),
            0x007dda48: b("08 f0 af 52"),
            0x007dda64: b("08 f0 af 52"),
            0x007dda80: b("08 f0 af 52"),
            0x007ddcc0: b("08 f0 af 52"),
            0x007ddcdc: b("08 f0 af 52"),
            0x007ddcf8: b("08 f0 af 52"),
            0x007dde00: b("08 f0 af 52"),
            0x007dde1c: b("08 f0 af 52"),
            0x007dde38: b("08 f0 af 52"),
            0x007ddf40: b("08 f0 af 52"),
            0x007ddf5c: b("08 f0 af 52"),
            0x007ddf78: b("08 f0 af 52"),
            0x007de334: b("08 f0 af 52"),
            0x007de350: b("08 f0 af 52"),
            0x007de36c: b("08 f0 af 52"),
            #0x00852d68: b("08 f0 af 52"), # this is related to something else? local_d0 = (local_d0 + 5000.0) / 10000.0; fStack_cc = (fStack_cc + 4000.0) / 8000.0;
            #0x0085302c: b("08 f0 af 52"), # w23, 10k 8k pair
            #0x00a7abc4: b("08 f0 af 52"), # w9, something storing multiples of 8k but seems grass/terrain/tile related
            #0x00b6d390: b("08 f0 af 52"), # gnarly terrain function, can't even find usage in decomp
            #0x00c48ac4: b("08 f0 af 52"), #      10k 8k pair
            0x00c570b8: b("0b f0 af 52"), # w11, something about aglsky params?
            0x00cb9524: b("08 f0 af 52"),
            #0x00f35d44: b("08 f0 af 52"), # x8, unrelated audio stuff
            0x011b3818: b("09 f0 af 52"), # w9
            0x0134b9b4: b("08 f0 af 52"),
            0x0134b9d0: b("08 f0 af 52"),
            0x0134b9ec: b("08 f0 af 52"),
            0x0134ea24: b("08 f0 af 52"),
            0x0134ea40: b("08 f0 af 52"),
            0x0134ea5c: b("08 f0 af 52"),
            0x0134ebb4: b("08 f0 af 52"),
            0x0134ebd0: b("08 f0 af 52"),
            0x0134ebec: b("08 f0 af 52"),
            #0x0175c800: b("08 f0 af 52"), #      10k 8k pair
        },
    },
    "lotuskit-fixed-20fps": {
        # Patch is applied by lotuskit tas when needed.
        # Writes large Framework.mGpuTimeStamp
        BuildId.TOTK_100: {
            0x02a7b654: b("08 02 a0 d2"), # mov x8,#0x10000
            0x02a7b658: b("68 8e 00 f9"), # str x8, [x19, #0x118]
            0x02a7b65c: NOP,
            0x02a7b660: NOP,
            0x02a7b664: NOP,
        },
        BuildId.TOTK_110: {
            0x02af47c4: b("08 02 a0 d2"), # mov x8,#0x10000
            0x02af47c8: b("68 8e 00 f9"), # str x8, [x19, #0x118]
            0x02af47cc: NOP,
            0x02af47d0: NOP,
            0x02af47d4: NOP,
        },
        BuildId.TOTK_121: {
            0x02aed814: b("08 02 a0 d2"), # mov x8,#0x10000
            0x02aed818: b("68 8e 00 f9"), # str x8, [x19, #0x118]
            0x02aed81c: NOP,
            0x02aed820: NOP,
            0x02aed824: NOP,
        },
    },
    "lotuskit-fixed-30fps": {
        # Patch is applied by lotuskit tas when needed.
        # Writes small Framework.mGpuTimeStamp
        BuildId.TOTK_100: {
            0x02a7b654: b("28 00 80 d2"), # mov x8, #0x1           vanilla: 08 00 08 cb  sub  x8, x0,x8
            0x02a7b658: b("68 8e 00 f9"), # str x8, [x19, #0x118]  vanilla: 69 8e 40 f9  ldr  x9, [x19, #0x118]
            0x02a7b65c: NOP,                                     # vanilla: 3f 01 08 eb  cmp  x9, x8
            0x02a7b660: NOP,                                     # vanilla: 4a 00 00 54  b.ge LAB_7102a7b668
            0x02a7b664: NOP,                                     # vanilla: 68 8e 00 f9  str  x8, [x19, #0x118]
        },
        BuildId.TOTK_110: {
            0x02af47c4: b("28 00 80 d2"), # mov x8, #0x1
            0x02af47c8: b("68 8e 00 f9"), # str x8, [x19, #0x118]
            0x02af47cc: NOP,
            0x02af47d0: NOP,
            0x02af47d4: NOP,
        },
        BuildId.TOTK_121: {
            0x02aed814: b("28 00 80 d2"), # mov x8, #0x1
            0x02aed818: b("68 8e 00 f9"), # str x8, [x19, #0x118]
            0x02aed81c: NOP,
            0x02aed820: NOP,
            0x02aed824: NOP,
        },
    },
}

def main():
    folder_name = os.path.join(os.path.dirname(__file__), "exefs_patches")
    pathlib.Path(folder_name).mkdir(parents=True, exist_ok=True)
    os.chdir(folder_name)
    write_ips_patchsets(LOTUSKIT_EXEFS_PATCHES)

    folder_name = os.path.join(os.path.dirname(__file__), "runtime_patches")
    pathlib.Path(folder_name).mkdir(parents=True, exist_ok=True)
    os.chdir(folder_name)
    write_ips_patchsets(LOTUSKIT_RUNTIME_PATCHES)

    if "install" in sys.argv:
        os.chdir(os.path.dirname(__file__))
        os.system("rsync -r exefs_patches/ ~/appdata/Roaming/Ryujinx/sdcard/atmosphere/exefs_patches/")
        os.system("rsync -r runtime_patches/ ~/appdata/Roaming/Ryujinx/sdcard/atmosphere/contents/0100f2c0115b6000/runtime_patches/")

if __name__ == "__main__":
    main()

