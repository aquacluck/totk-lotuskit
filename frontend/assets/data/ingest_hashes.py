#!/usr/bin/env python3
import json
import os
os.chdir(os.path.dirname(__file__))

# ty Fredrik Kihlander + Swapnil Gusani https://github.com/wc-duck/pymmh3/blob/master/pymmh3.py
# TODO Bool64bitKey 64b hash+lookup
def murmur32(key, seed = 0x0):
    ''' Implements 32bit murmur3 hash. '''
    if isinstance(key, bytes) or isinstance(key, bytearray):
        key = bytearray(key)
    else:
        key = bytearray(key.encode())

    def fmix( h ):
        h ^= h >> 16
        h  = ( h * 0x85ebca6b ) & 0xFFFFFFFF
        h ^= h >> 13
        h  = ( h * 0xc2b2ae35 ) & 0xFFFFFFFF
        h ^= h >> 16
        return h

    length = len( key )
    nblocks = int( length / 4 )

    h1 = seed

    c1 = 0xcc9e2d51
    c2 = 0x1b873593

    # body
    for block_start in range( 0, nblocks * 4, 4 ):
        # ??? big endian?
        k1 = key[ block_start + 3 ] << 24 | \
             key[ block_start + 2 ] << 16 | \
             key[ block_start + 1 ] <<  8 | \
             key[ block_start + 0 ]

        k1 = ( c1 * k1 ) & 0xFFFFFFFF
        k1 = ( k1 << 15 | k1 >> 17 ) & 0xFFFFFFFF # inlined ROTL32
        k1 = ( c2 * k1 ) & 0xFFFFFFFF

        h1 ^= k1
        h1  = ( h1 << 13 | h1 >> 19 ) & 0xFFFFFFFF # inlined ROTL32
        h1  = ( h1 * 5 + 0xe6546b64 ) & 0xFFFFFFFF

    # tail
    tail_index = nblocks * 4
    k1 = 0
    tail_size = length & 3

    if tail_size >= 3:
        k1 ^= key[ tail_index + 2 ] << 16
    if tail_size >= 2:
        k1 ^= key[ tail_index + 1 ] << 8
    if tail_size >= 1:
        k1 ^= key[ tail_index + 0 ]

    if tail_size > 0:
        k1  = ( k1 * c1 ) & 0xFFFFFFFF
        k1  = ( k1 << 15 | k1 >> 17 ) & 0xFFFFFFFF # inlined ROTL32
        k1  = ( k1 * c2 ) & 0xFFFFFFFF
        h1 ^= k1

    #finalization
    unsigned_val = fmix( h1 ^ length )
    return unsigned_val # XXX all our tooling expects these to be unsigned
    '''
    if unsigned_val & 0x80000000 == 0:
        return unsigned_val
    else:
        return -( (unsigned_val ^ 0xFFFFFFFF) + 1 )
    '''

#from collections import defaultdict
#from dataclasses import dataclass
from typing import *

HASH_TYPES = (
    "Enum",
    "EnumArray",
    "Struct",
    "StructArray",
    # TODO struct member types?

    "Binary",
    "BinaryArray",

    "Bool",
    "BoolArray",
    "Bool64bitKey",
    "BoolExp",
    "SaveTypeHash",

    "Int",
    "IntArray",
    "Int64",
    "Int64Array",
    "UInt",
    "UIntArray",
    "UInt64",
    "UInt64Array",

    "Float",
    "FloatArray",

    "Vector2",
    "Vector2Array",
    "Vector3",
    "Vector3Array",
    "Vector3f",

    "StringArray",
    "String16",
    "String16Array",
    "String32",
    "String32Array",
    "String64",
    "String64Array",

    "WString16",
    "WString16Array",
    "WString32",
    "WString32Array",
    "WString64",
    "WString64Array",
)

global_hashinfo: Dict[int, "GameDataHashInfo"] = {}
class GameDataHashInfo:
    def __init__(self):
        self.hash: int = None
        self.key: str = None
        self.types = set()
        self.comment = ""

        # TODO index struct member types?
        self.struct_members = set() # When self.key is a struct, it may contain these members
        self.struct_owners = set() # Occurences where self.key is a member of some struct

        # self.enum_owned_type = str # XXX do enums ever collide across types or resolve to complex values? seems like they might always be hash->enumindex?
                                     # if so, enum_ownership might need to be scoped to type Dict[type, set(int)]
                                     # dont understand this yet

        # self.enum_ownership: List[int] = [] # When self.key is an enum, it may contain these members

    def __repr__(self):
        k = self.key if self.key else "[unknown]"
        t = "|".join(self.types)
        return f"GameDataHashInfo({self.hash}:{k})<{t}>"

    def __hash__(self):
        return self.hash

    def set_key(self, key: str):
        # don't allow mismatching keys
        hash = murmur32(key)
        assert hash == self.hash, f"[hashinfo] murmur32({key}) -> {hash} != self {self.hash}"
        self.key = key

    @classmethod
    def get_or_create(cls, hash=None, key=None):
        if key is not None and hash is not None:
            assert murmur32(key) == hash, "[hashinfo] hash+key mismatch"
        elif hash is None:
            assert key is not None, "[hashinfo] hash+key cannot both be None"
            hash = murmur32(key)
        assert hash is not None, "[hashinfo] hash unavailable"

        global global_hashinfo
        entry = global_hashinfo.get(hash)
        if entry is None:
            entry = cls()
            global_hashinfo[hash] = entry
            # ctor
            entry.hash = hash
            entry.key = key
        return entry

    @classmethod
    def ingest_dotted_key(cls, dotted_key: str, typename: str = None):
        if typename:
            assert typename in HASH_TYPES, f"[ingest_dotted_key] invalid type {typename}"
            cls.get_or_create(key=dotted_key).types.add(typename)
        if "." not in dotted_key:
            return

        segments = dotted_key.split(".")
        seg_i = -1
        for seg in segments:
            seg_i += 1
            member_entry = cls.get_or_create(key=seg)
            if typename and seg == segments[-1]:
                member_entry.types.add(typename)
            containing_key = ".".join(segments[0:seg_i])
            if not containing_key:
                continue # top level segment
            containing_entry = cls.get_or_create(key=containing_key)
            containing_entry.types.add("Struct") # XXX does this cover everything, eg could this be Enum or StructArray?
            containing_entry.struct_members.add(member_entry.hash)
            member_entry.struct_owners.add(containing_entry.hash)

    @classmethod
    def map_hash_to_key_or_hex(cls, hash: int):
        entry = cls.get_or_create(hash=hash)
        if entry.key:
            return entry.key
        return hex(hash)



# https://raw.githubusercontent.com/MrCheeze/totk-tools/refs/heads/master/gamedata_hashes.txt 9ea767ee6e15b1c1b6f3a57a95bd3e5f0b5bf0cc
def ingest_mrcheeze():
    with open("gamedata_hashes.txt", "r") as srcfile:
        # TODO Bool64bitKey
        srctype = None
        struct_member_i = 0
        struct_entry = None
        line_i = 0
        for line in srcfile:
            line = line.rstrip("\n") # lol python
            line_i += 1
            if line.startswith("--- ") and line.endswith(" ---"):
                # change type
                srctype = line[4:-4]
                assert srctype in HASH_TYPES, f"[mrcheeze] invalid type {srctype}"
                continue

            if srctype == "Struct":
                l = line.split("\t")
                if l[0] != "":
                    # open struct
                    struct_entry = GameDataHashInfo.get_or_create(int(l[0]))
                    struct_entry.types.add(srctype)
                    struct_member_i = 0
                    if len(l) > 1:
                        struct_entry.set_key(l[1])
                        GameDataHashInfo.ingest_dotted_key(l[1], srctype)
                    continue

                else:
                    # struct member
                    struct_member_i += 1
                    member_entry = GameDataHashInfo.get_or_create(int(l[1]))
                    # no type info to add to struct member?
                    if len(l) > 2:
                        member_entry.set_key(l[2])
                        GameDataHashInfo.ingest_dotted_key(l[2], srctype)
                    struct_entry.struct_members.add(int(l[1]))
                    member_entry.struct_owners.add(struct_entry.hash)
                    continue

            else:
                l = line.split("\t")
                entry = GameDataHashInfo.get_or_create(int(l[0]))
                entry.types.add(srctype)
                if len(l) > 1:
                    entry.set_key(l[1])
                    GameDataHashInfo.ingest_dotted_key(l[1], srctype)
                continue

# https://raw.githubusercontent.com/McSpazzy/totk-gamedata/refs/heads/master/GameDataListFull.csv a06f836ad6d3f8c42631263e8a5b0f850f06635b
def ingest_mcspazzy():
    with open("GameDataListFull.csv", "r") as srcfile:
        line_i = -1
        for line in srcfile:
            line = line.rstrip("\n") # lol python
            line_i += 1
            # 004343,D51BFCEB,3575381227,Bool,HavePlayedEvent.78328533_3858284904_2969203814_D_T
            line_i_src, hexhashstr, inthashstr, srctype, key = line.split(",")
            assert line_i == int(line_i_src, 10), f"[mcspazzy] line mismatch {line_i} != {line_i_src}"
            hash = int(hexhashstr, 16)
            assert hash == int(inthashstr, 10), f"[mcspazzy] hash mismatch {hexhashstr} != {inthashstr}"
            comment = None
            if key == "Unknown":
                key = None
            elif key.startswith("TempName."):
                comment = "partial (unknown prefix): " + key
                key = None
            elif "|" in key:
                key, comment = key.split("|")
            if key:
                assert hash == murmur32(key), f"[mcspazzy] key mismatch murmur32({key}) != {hash}"

            entry = GameDataHashInfo.get_or_create(hash)
            entry.types.add(srctype)
            if key:
                entry.set_key(key)
                GameDataHashInfo.ingest_dotted_key(key, srctype)
            if comment:
                entry.comment += comment

# https://raw.githubusercontent.com/marcrobledo/savegame-editors/refs/heads/master/zelda-totk/zelda-totk.hashes.csv ae8c4388d5ae97acccb5955d3734ff02408a25d8
def ingest_marc():
    with open("zelda-totk.hashes.csv", "r") as srcfile:
        line_i = -1
        for line in srcfile:
            line = line.rstrip("\n") # lol python
            line_i += 1
            if line == "" or line.startswith("#"):
                continue
            l = line.split(";")
            ln = len(l)
            assert ln == 3 or ln == 4
            if l[0] == "EnumValues":
                pass # TODO
                continue
            else:
                hash = int(l[0], 16)
                srctype = l[1]
                assert srctype in HASH_TYPES
                key = l[2]
                comment = "" if ln == 3 else l[3]
                if key == "Unknown":
                    key = None
                elif key.startswith("TempName."):
                    comment += "partial (unknown prefix): " + key
                    key = None

                entry = GameDataHashInfo.get_or_create(hash=hash)
                entry.types.add(srctype)
                if key:
                    entry.set_key(key)
                    GameDataHashInfo.ingest_dotted_key(key, srctype)
                if entry.comment != comment:
                    entry.comment += comment


def output_jsdata():
    with open("GMDHash_InfoList.mjs", "w") as outfile:
        outfile.write("const GMDHash_InfoList_raw = [\n")
        for (hash, entry) in global_hashinfo.items():
            out = { "hash": hash, "hex": hex(hash) }
            # TODO BigInt literals for 64b hashes?
            if entry.key:
                out["key"] = entry.key
            if entry.types:
                out["types"] = list(entry.types)
            if entry.comment:
                out["comment"] = entry.comment
            if entry.struct_members:
                out["members"] = list(map(GameDataHashInfo.map_hash_to_key_or_hex, entry.struct_members))
            if entry.struct_owners:
                out["owners"] = list(map(GameDataHashInfo.map_hash_to_key_or_hex, entry.struct_owners))
            outfile.write(json.dumps(out))
            outfile.write(",\n")
        outfile.write("];\n\n")
        outfile.write('''
            function GMDHash_InfoList_prepare_fuzzysort(fuzzysort) {
                return GMDHash_InfoList_raw.map(h => {
                    let out = {hash: fuzzysort.prepare(h.hash), hex: fuzzysort.prepare(h.hex)};
                    if (h.key) { out.key = fuzzysort.prepare(h.key); }
                    if (h.types) { out.types = fuzzysort.prepare(h.types.join(",")); }
                    if (h.comment) { out.comment = fuzzysort.prepare(h.comment); }
                    if (h.members) { out.members = fuzzysort.prepare(h.members.join(",")); }
                    if (h.owners) { out.owners = fuzzysort.prepare(h.owners.join(",")); }
                    return out;
                });
            }

            export {GMDHash_InfoList_raw, GMDHash_InfoList_prepare_fuzzysort};
        ''')


if __name__ == "__main__":
    # TODO just ingest this stuff directly idk
    ingest_mrcheeze()
    ingest_mcspazzy()
    ingest_marc()
    #breakpoint()
    output_jsdata()



