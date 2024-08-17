from __future__ import annotations
from typing import *
from dataclasses import dataclass
from collections import defaultdict

from build_helper_itanium_demangler import Node as SymAST
from build_helper_itanium_demangler import parse as sym_demangle


class MagicWords:
    ALL = "all"
    SKIP = "skip"

class NSOModule:
    # separate sym_map per module
    sym_map: Dict[RuntimeSymbol.name, RuntimeSymbol] = NotImplementedError

    # hold all nso modules, just do this by hand
    ALL_MODULES = []

    def __new__(cls, *args, **kwargs) -> RuntimeSymbol:
        # sugar to accumulate symbols in cls.sym_map
        sym = RuntimeSymbol(*args, **kwargs, module=cls)
        cls.sym_map[sym.name] = sym
        return sym

class RTLD(NSOModule):
    nso_filename = "rtld"
    module_name = "nnrtld"
    module_start_sym = MagicWords.SKIP
    sym_map = {}

class EXKING(NSOModule):
    nso_filename = "main"
    module_name = "EX-King"
    module_start_sym = "__main_start"
    sym_map = {}
    #module_filename = r"D:\home\Project\EX-King\App\Rom\NX64\Product_Optimize\code\EX-King.nss"

class MULTIMEDIA(NSOModule):
    nso_filename = "subsdk0"
    module_name = "multimedia"
    module_start_sym = MagicWords.SKIP
    sym_map = {}

class NNSDK(NSOModule):
    nso_filename = "sdk"
    module_name = "nnSdk"
    module_start_sym = "__sdk_start"
    sym_map = {}

class LOTUSKIT(NSOModule):
    nso_filename = "subsdk9"
    module_name = "subsdk9" # FIXME can i set name?
    module_start_sym = MagicWords.SKIP
    sym_map = {}

NSOModule.ALL_MODULES.extend([RTLD, EXKING, MULTIMEDIA, NNSDK, LOTUSKIT])

class GameVersion:
    ALL_STR = ("TOTK_100", "TOTK_110", "TOTK_111", "TOTK_112", "TOTK_120", "TOTK_121")
    TOTK_100 = 100
    TOTK_110 = 110
    TOTK_111 = 111
    TOTK_112 = 112
    TOTK_120 = 120
    TOTK_121 = 121

    @classmethod
    def to_str(cls, version: int) -> str:
        for version_str in cls.ALL_STR:
            if version == getattr(cls, version_str):
                return version_str
        raise ValueError(f"GameVersion {version} invalid")

    @classmethod
    def to_i(cls, version: str) -> int:
        if version not in GameVersion.ALL_STR:
            raise ValueError(f"GameVersion {version} invalid")
        return getattr(GameVersion, version)

TOTK_100 = GameVersion.TOTK_100
TOTK_110 = GameVersion.TOTK_110
TOTK_111 = GameVersion.TOTK_111
TOTK_112 = GameVersion.TOTK_112
TOTK_120 = GameVersion.TOTK_120
TOTK_121 = GameVersion.TOTK_121


@dataclass
class RuntimeSymbol:
    mangled: str
    address: VersionedAddress
    module: NSOModule

    def __post_init__(self):
        # set derived
        self.ast: SymAST = sym_demangle(self.mangled)
        self.name = str(self.ast)
        self.address = VersionedAddress.from_pydsl(self.address)


class VersionedAddress:
    @classmethod
    def from_pydsl(cls, raw: dict) -> VersionedAddress:
        ret = VersionedAddress()
        if not raw:
            return ret

        for k, v in raw.items():
            if k == MagicWords.ALL:
                # dict keys can be specified in any order.
                # ALL_TOKEN assigns to all nullptr'd versions at that time.
                for ka in GameVersion.ALL_STR:
                    if ret[ka] == 0:
                        ret[ka] = v
            else:
                ret[k] = v

        return ret

    def __init__(self):
        self.version_map: defaultdict[GameVersion, int] = defaultdict(int)

    def __getitem__(self, key) -> int:
        if type(key) == str:
            key: int = GameVersion.to_i(key)
        GameVersion.to_str(key) # assert
        return self.version_map[key]

    def __setitem__(self, key, value):
        if type(key) == str:
            key: int = GameVersion.to_i(key)
        GameVersion.to_str(key) # assert
        self.version_map[key] = value

