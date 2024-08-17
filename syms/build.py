#!/usr/bin/env python3
from __future__ import annotations

# these module types store symbols as they're declared
from build_types import *


def run_symbol_def_imports():
    # these imports are registering symbols
    import sead.random.seadGlobalRandom
    import sead.random.seadRandom


class TrashCommands:
    """
    @staticmethod
    def list_module_syms(mod: NSOModule):
        print(f"{mod.module_name} count: {len(mod.sym_map)}")
        for name, sym in mod.sym_map.items():
            print(f"{name} -> {sym.address}")

    @staticmethod
    def list_all_module_syms():
        for mod in NSOModule.ALL_MODULES:
            TrashCommands.list_module_syms(mod)
    """

    @staticmethod
    def build_gameversion_syms(version: GameVersion, do_export_nullptr=True):
        # TODO would it be useful to output unmerged syms, maybe mirroring input fs structure?
        with open(f"output/{version}/syms_merged.ld", "w") as symfile:
            version_str = GameVersion.to_str(version)
            symfile.write(f"__game_version = {version}; /* {version_str} */")
            for mod in NSOModule.ALL_MODULES:
                if mod.module_start_sym == MagicWords.SKIP:
                    continue

                symfile.write(f"\n\n/* ======== BEGIN {mod.module_name} (nso {mod.nso_filename}, symbol {mod.module_start_sym}) =========== */\n\n")
                for name, sym in mod.sym_map.items():
                    addr = sym.address[version]
                    if addr == MagicWords.SKIP:
                        continue
                    if addr == 0 and not do_export_nullptr:
                        continue

                    if addr <= 0xffffffff:
                        addr = f"        {addr:#010x}"
                    else:
                        addr = f"{addr:#018x}"
                    symfile.write(f"{sym.mangled:32} = {mod.module_start_sym} + {addr}; /* {sym.name} */\n")
                symfile.write(f"\n/* ======== END   {mod.module_name} (nso {mod.nso_filename}) ================================ */\n\n")


if __name__ == "__main__":
    run_symbol_def_imports()
    #breakpoint()
    #m = EXKING; print(f"{m.module_name} {len(m.sym_map)}")
    #TrashCommands.list_module_syms(EXKING)
    #TrashCommands.list_all_module_syms()
    for v in GameVersion.ALL_STR:
        TrashCommands.build_gameversion_syms(GameVersion.to_i(v))

