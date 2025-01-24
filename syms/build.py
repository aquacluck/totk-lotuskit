#!/usr/bin/env python3
from __future__ import annotations
import os.path
import pathlib


# these module types store symbols as they're declared
from build_types import *

def run_symbol_def_imports():
    # these imports are registering symbols.
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
    import exking.engine.actor.ActorMgr
    import exking.engine.actor.BaseProcCreateAndDeleteThread
    import exking.engine.actor.BaseProcLink
    import exking.engine.actor.BaseProcMgr
    import exking.engine.event.EventMgr
    import exking.engine.hacks
    import exking.engine.module.VFRMgr

    import exking.game.ai.execute.ExecutePlayerWhistle
    import exking.game.ai.execute.ExecutePlayerReverseRecorder
    import exking.game.wm.WorldManagerModule
    import exking.game.component.GameCameraParam

    import exking.phive.CharacterMatterRigidBodyEntity
    import exking.phive.ControllerSet
    import exking.phive.RigidBodyBase
    import exking.phive.RigidBodyEntity

    import havok.hk.hacks


class TrashCommands:
    @staticmethod
    def build_gameversion_syms(version: GameVersion, do_export_nullptr=True):
        version_str = GameVersion.to_str(version)
        pathlib.Path(f"output/{version_str}").mkdir(parents=True, exist_ok=True)
        with open(f"output/{version_str}/syms_exl_reloc.inc", "w") as hppfile, open(f"output/{version_str}/syms_merged.ld", "w") as ldfile:
            ldfile.write(f"__game_version = {version}; /* {version_str} */")

            for mod in NSOModule.ALL_MODULES:
                if mod.module_start_sym == MagicWords.SKIP:
                    continue

                hppfile.write(f"\n\n/* ======== BEGIN {mod.module_name} ({mod.nso_filename}) =========== */\n\n")
                ldfile.write( f"\n\n/* ======== BEGIN {mod.module_name} (nso {mod.nso_filename}, symbol {mod.module_start_sym}) =========== */\n\n")

                if mod.module_start_offset and mod.module_start_offset != MagicWords.SKIP:
                    ldfile.write(f"{mod.module_start_sym}_offset = {mod.module_start_offset[version]:#x};\n\n")

                for sym in mod.sym_map.values():
                    offset = sym.address[version]
                    if offset == MagicWords.SKIP:
                        continue
                    if offset == 0 and do_export_nullptr is False:
                        continue

                    offset_str = f"{offset:#010x}" if offset <= 0xffffffff else f"{offset:#018x}" # does this ever branch?

                    if sym.is_mangled:
                        # TODO demangle for hpp lookup? if the need arises
                        ldfile.write(f"{sym.unique_name:32} = {mod.module_start_sym} + {offset_str};\n")

                    # include everything in hpp -- {{ is escaped { for f-strings
                    hppfile.write(f'''{{ {mod.exl_util_module_index}, {offset_str}, "{sym.unique_name}" }},\n''')

                hppfile.write(f"\n/* ======== END {mod.module_name} ({mod.nso_filename}) ============== */\n\n")
                ldfile.write( f"\n/* ======== END {mod.module_name} (nso {mod.nso_filename}) ============== */\n\n")

            hppfile.write(f'''{{ util::ModuleIndex::Main, 0x0, "__null_unused__" }}\n''') # fix trailing comma


if __name__ == "__main__":
    # work in syms folder regardless of initial cwd
    os.chdir(os.path.dirname(__file__))

    run_symbol_def_imports()
    [module.postprocess() for module in NSOModule.ALL_MODULES]
    for v in GameVersion.ALL_STR:
        TrashCommands.build_gameversion_syms(GameVersion.to_i(v))

