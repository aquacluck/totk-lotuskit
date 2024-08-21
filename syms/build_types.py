from __future__ import annotations
from typing import *
from collections import defaultdict
import os.path
import pathlib

from build_helper_itanium_demangler import Node, QualNode, CastNode, FuncNode, ArrayNode, MemberNode
from build_helper_itanium_demangler import parse as sym_demangle


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


class VersionedAddress:
    @classmethod
    def from_pydsl(cls, raw: dict) -> VersionedAddress:
        ret = VersionedAddress()
        if not raw:
            return ret

        for k, v in raw.items():
            if k == MagicWords.ALL:
                # dict keys can be specified in any order.
                # ALL assigns to all nullptr'd versions at that time.
                for ka in GameVersion.ALL_STR:
                    if ret[ka] == 0:
                        ret[ka] = v
            else:
                ret[k] = v

        return ret

    def __init__(self):
        self.version_map: defaultdict[GameVersion, int] = defaultdict(int)

    def __getitem__(self, key) -> int:
        if isinstance(key, str):
            key: int = GameVersion.to_i(key)
        GameVersion.to_str(key) # assert
        return self.version_map[key]

    def __setitem__(self, key, value):
        if isinstance(key, str):
            key: int = GameVersion.to_i(key)
        GameVersion.to_str(key) # assert
        self.version_map[key] = value


current_sym_file: Optional[Tuple[str]] = None
def begin_sym_file(dsl__file__):
    # record current file to annotate RuntimeSymbol
    abs_dir_prefix = os.path.abspath(os.path.dirname(__file__)) # XXX don't move this into a folder
    dsl_relative = os.path.abspath(os.path.dirname(dsl__file__)).removeprefix(abs_dir_prefix).strip("/\\")
    dsl_relative_parts = pathlib.Path(dsl_relative).parts
    output_basename = os.path.basename(dsl__file__).removesuffix(".py")
    output = dsl_relative_parts + (output_basename,) # eg ('sead', 'heap', 'seadHeapMgr')

    global current_sym_file
    current_sym_file = output


class SymbolType:
    FUNCTION = "FUNCTION" # default
    INSTRUCTION = "INSTRUCTION" # usually for inline hooking
    DATA = "DATA" # non executable

class MagicWords:
    ALL = "ALL"
    SKIP = "SKIP"

ALL = MagicWords.ALL
SKIP = MagicWords.SKIP


class NSOModule:
    # separate sym_map per module
    sym_map: Dict[RuntimeSymbol.name, RuntimeSymbol] = NotImplementedError

    # hold all nso modules, just do this by hand
    ALL_MODULES = []

    def __new__(cls, mangled, address, symbol_type=SymbolType.FUNCTION) -> RuntimeSymbol:
        # sugar to accumulate symbols in cls.sym_map
        global current_sym_file
        sym = RuntimeSymbol(mangled, address, symbol_type, module=cls, source_path=current_sym_file)
        cls.sym_map[sym.name] = sym
        return sym

    @classmethod
    def unique_source_paths(cls) -> Tuple[Tuple[str]]:
        return tuple(sorted({ s.source_path for s in cls.sym_map.values() }))

    @classmethod
    def postprocess(cls):
        # ensure stable output order
        keys = list(cls.sym_map.keys())
        keys.sort(key=lambda k: cls.sym_map[k].mangled)
        tmp: dict = cls.sym_map
        cls.sym_map: dict = {k: tmp[k] for k in keys}
        del tmp

class RTLD(NSOModule):
    nso_filename = "rtld"
    module_name = "nnrtld"
    module_start_sym = "__rtld_start"
    module_start_offset = MagicWords.SKIP # constant 0x4000 before main
    sym_map = {}

class EXKING(NSOModule):
    nso_filename = "main"
    module_name = "EX-King"
    module_start_sym = "__main_start"
    module_start_offset = VersionedAddress.from_pydsl({
        # applied in subsdk9.ld to find main module
        TOTK_100: 0x04d0f000,
        TOTK_110: 0x04dd6000,
        TOTK_111: 0x04dde000,
        TOTK_112: 0x04dd0000,
        TOTK_120: 0x04dc5000,
        TOTK_121: 0x04dd4000,
    })
    sym_map = {}
    #module_filename = r"D:\home\Project\EX-King\App\Rom\NX64\Product_Optimize\code\EX-King.nss"

class MULTIMEDIA(NSOModule):
    nso_filename = "subsdk0"
    module_name = "multimedia"
    module_start_sym = "__subsdk0_start"
    module_start_offset = MagicWords.SKIP # TODO if we want
    sym_map = {}

class LOTUSKIT(NSOModule):
    nso_filename = "subsdk9"
    module_name = "subsdk9" # FIXME can i set name?
    module_start_sym = MagicWords.SKIP
    module_start_offset = MagicWords.SKIP
    sym_map = {}

class NNSDK(NSOModule):
    nso_filename = "sdk"
    module_name = "nnSdk"
    module_start_sym = "__sdk_start"
    module_start_offset = MagicWords.SKIP # location inferred from end of subsdk9
    sym_map = {}

NSOModule.ALL_MODULES.extend([RTLD, EXKING, MULTIMEDIA, LOTUSKIT, NNSDK])


class RuntimeSymbol:
    # supplied from source_path (symbol file)
    mangled: str
    address: VersionedAddress
    symbol_type: SymbolType

    # supplied by build system
    module: NSOModule
    source_path: Tuple[str]

    def __init__(self, mangled, address, symbol_type, module, source_path):
        self.mangled = mangled
        self.address = address
        self.symbol_type = symbol_type
        self.module = module
        self.source_path = source_path

        # set derived
        self.ast: Node = sym_demangle(self.mangled)
        self.name = str(self.ast)
        self.address = VersionedAddress.from_pydsl(self.address)
        self._derive_ns()

    def __hash__(self):
        return hash(self.mangled)

    def _derive_ns(self):
        self.ns = () # default: no namespace
        self.subject_identifier = None # the "basename" identifier, eg the function name

        if self.ast.kind == "qual_name":
            ns = []
            for ns_identifier in self.ast.value[:-1]:
                if ns_identifier.kind != "name":
                    e = NotImplementedError(f"unsupported ns_identifier kind {ns_identifier.kind} in {type(self.ast)}: {self.name}")
                    breakpoint(); raise e
                ns.append(ns_identifier.value)

            self.subject_identifier = self.ast.value[-1].value
            self.ns = tuple(ns)
            return

        elif isinstance(self.ast, FuncNode):
            namenode = self.ast.name
            if namenode.kind == "name":
                self.subject_identifier = namenode.value
                return # unqualified: no namespace

            if namenode.kind != "qual_name":
                e = NotImplementedError(f"unsupported name kind {namenode.kind} in {type(self.ast)}: {self.name}")
                breakpoint(); raise e

            ns = []
            for ns_identifier in namenode.value[:-1]:
                if ns_identifier.kind != "name":
                    e = NotImplementedError(f"unsupported ns_identifier kind {ns_identifier.kind} in {type(self.ast)}: {self.name}")
                    breakpoint(); raise e
                ns.append(ns_identifier.value)

            self.subject_identifier = namenode.value[-1].value
            self.ns = tuple(ns)
            return

        # handle more when it comes up
        e = NotImplementedError(f"unsupported root type {type(self.ast)}: {self.name}")
        breakpoint(); raise e
