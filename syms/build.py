#!/usr/bin/env python3
from __future__ import annotations
import os.path
import pathlib


# these module types store symbols as they're declared
from build_types import *

def run_symbol_def_imports():
    # these imports are registering symbols.
    # symbol folder structure mirrors dt_totk include paths -- first folder component (sead, NintendoSDK, zstd) gets included by cmake.
    # the folder structure is solely to make folders match up + make the output nicer to navigate
    # vs namespaces are defined solely by mangled symbols (independent of folder structure)

    # TODO just crawl some top level folders, this is getting stupid already

    # TODO eventflow

    #import NintendoSDK.nn.socket # nop

    import agl.agl_hacks
    import agl.lyr.aglLayer

    import sead.gfx.seadPrimitiveRenderer
    import sead.gfx.seadTextWriter
    import sead.random.seadGlobalRandom
    import sead.random.seadRandom
    import sead.heap.seadHeap # nop
    import sead.heap.seadHeapMgr
    import sead.thread.seadThread

    #import zstd.lib.zstd # nop


    import exking.engine.actor.ActorBase
    import exking.engine.actor.ActorInstanceMgr
    import exking.engine.actor.ActorMgr
    import exking.engine.actor.BaseProcCreateAndDeleteThread
    import exking.engine.hacks
    import exking.engine.module.VFRMgr

    import exking.game.ai.execute.ExecutePlayerWhistle
    import exking.game.ai.execute.ExecutePlayerReverseRecorder
    import exking.game.wm.WorldManagerModule

    import exking.phive.ControllerSet
    import exking.phive.RigidBodyBase

    import havok.hk.hacks


class TrashCommands:
    @staticmethod
    def build_gameversion_syms(version: GameVersion, do_export_nullptr=True):
        version_str = GameVersion.to_str(version)
        pathlib.Path(f"output/{version_str}").mkdir(parents=True, exist_ok=True)
        with open(f"output/{version_str}/syms_merged.hpp", "w") as hppfile, open(f"output/{version_str}/syms_merged.ld", "w") as ldfile:
            hppfile.write(f'#pragma once\n#include <cstddef>\n\n') #hppfile.write(f'#pragma once\n#include "lib/json.hpp"\nusing json = nlohmann::json;\n\n')
            ldfile.write(f"__game_version = {version}; /* {version_str} */")

            for mod in NSOModule.ALL_MODULES:
                if mod.module_start_sym == MagicWords.SKIP:
                    continue

                hppfile.write(f"\n\n/* ======== BEGIN {mod.module_name} (nso {mod.nso_filename}, symbol {mod.module_start_sym}) =========== */\n\n")
                ldfile.write( f"\n\n/* ======== BEGIN {mod.module_name} (nso {mod.nso_filename}, symbol {mod.module_start_sym}) =========== */\n\n")

                if mod.module_start_offset and mod.module_start_offset != MagicWords.SKIP:
                    ldfile.write(f"{mod.module_start_sym}_offset = {mod.module_start_offset[version]:#x};\n\n")

                for sym in mod.sym_map.values():
                    addr = sym.address[version]
                    if addr == MagicWords.SKIP:
                        continue
                    if addr == 0 and do_export_nullptr is False:
                        continue

                    if addr <= 0xffffffff:
                        addr_str = f"{addr:#010x}"
                    else:
                        addr_str = f"{addr:#018x}"

                    if sym.symbol_type not in (SymbolType.DATA, SymbolType.INSTRUCTION):
                        # only func ptrs are relevant for ld symbols?
                        ldfile.write(f"{sym.mangled:32} = {mod.module_start_sym} + {addr_str}; /* {sym.name} */\n")

                    # TODO module_start_offset for xmodule hooks?
                    # include everything in hpp -- {{ is escaped { for f-strings
                    identifier = sym.subject_identifier or sym.mangled
                    # TODO how to expose syms across cpp+as+js -- trivial to access in cpp, trivial to register to as, trivial to forward over ws for js ui
                    '''
                    inline static json stuff = {{
                        {{"offset", {addr_str}}},
                        {{"mangled", "{sym.mangled}"}},
                        {{"demangled", "{sym.name}"}}
                    }};
                    '''

                    hppfile.write(f"""
namespace sym::{'::'.join(sym.ns)} {{
class {identifier} {{
    public:
    inline static constexpr ptrdiff_t offset = {addr_str};
}};
}}\n""")

                hppfile.write(f"\n/* ======== END {mod.module_name} (nso {mod.nso_filename}) ================================ */\n\n")
                ldfile.write( f"\n/* ======== END {mod.module_name} (nso {mod.nso_filename}) ================================ */\n\n")


if __name__ == "__main__":
    run_symbol_def_imports()
    [module.postprocess() for module in NSOModule.ALL_MODULES]
    for v in GameVersion.ALL_STR:
        TrashCommands.build_gameversion_syms(GameVersion.to_i(v))

