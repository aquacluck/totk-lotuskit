#!/usr/bin/env python3
import os, os.path, struct
os.chdir(os.path.dirname(__file__))

# nso main ModuleId/BuildId (up to 32B):
class BuildId:
    TOTK_100 = "082ce09b06e33a123cb1e2770f5f9147709033db"
    TOTK_110 = "d5ad6ac71ef53e3e52417c1b81dbc9b4142aa3b3"
    TOTK_121 = "9b4e43650501a4d4489b4bbfdb740f26af3cf850"

# 5B "IPS32" magic
# patch_records[]:
#     4B NSO offset (+0x100 from ghidra .text/MOD0)
#     2B patch_size
#     {patch_size}B patch_value
# 4B "EEOF"

def write_ips(filename: str, patches: dict, offset_shift=0x100):
    # IPSwitch at home
    fd = open(filename, "wb")
    fd.write(b"IPS32")
    for offset, value in patches.items():
        fd.write(struct.pack(">I", offset+offset_shift))
        assert len(value) <= 0xffff, "patch record payload must be <64KiB"
        fd.write(struct.pack(">H", len(value))) # fd.write(bytes(divmod(len(value), 0x100)))
        fd.write(bytes(value))
    fd.write(b"EEOF")
    fd.close()

def b(v) -> bytes: # ez input
    if type(v) is bytes:
        return v
    if type(v) is int:
        v: str = hex(v)
    v = str(v).lstrip("0x").replace(" ", "").replace("\t", "").replace("\r", "").replace("\n", "")
    if len(v) % 2:
        v = "0" + v # fromhex wants full bytes
    return bytes.fromhex(v)

def main():
    #b = bytes.fromhex

    write_ips(f"./nn-standardallocator-shrink-2MB/{BuildId.TOTK_100}.ips", {
        0x01130f28: b("02 6c a8 52"), # w2,#0x43600000 # vanilla - 2MB
        0x01130f2c: b("13 0e a0 52"), # w19,#0x700000  # vanilla - 2MB
        #0x01130f44: b"\x04\x6c\xa8\xd2", # mov x4,#0x43600000
    })

    write_ips(f"./nn-standardallocator-shrink-2MB/{BuildId.TOTK_110}.ips", {
        0x01160524: b"\x04\x64\xa8\xd2", # mov x4,#0x43200000
    })

    write_ips(f"./nn-standardallocator-shrink-2MB/{BuildId.TOTK_121}.ips", {
        0x01155ec4: b"\x04\x64\xa8\xd2", # mov x4,#0x43200000
    })

if __name__ == "__main__":
    main()

