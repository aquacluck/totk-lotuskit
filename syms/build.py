#!/usr/bin/env python3
from __future__ import annotations
import os.path
import pathlib

# these module types store symbols as they're declared
from build_types import *


def run_symbol_def_imports():
    # these imports are registering symbols
    import sead.random.seadGlobalRandom
    import sead.random.seadRandom
    import sead.heap.seadHeapMgr


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
    def build_gameversion_linkerscript_syms(version: GameVersion, do_export_nullptr=True):
        # TODO would it be useful to output unmerged syms, maybe mirroring input fs structure?
        version_str = GameVersion.to_str(version)
        with open(f"output_ld_script/{version_str}/syms_merged.ld", "w") as symfile:
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

    @staticmethod
    def build_gameversion_hpp_syms():
        for module in NSOModule.ALL_MODULES:
            for source_path in module.unique_source_paths():
                # TODO annotate each hpp with a module identifier? then we can hook anything by name
                source_syms: Set[RuntimeSymbol] = { sym for sym in module.sym_map.values() if sym.source_path == source_path }
                basename_output = source_path[-1] + ".hpp"

                mkdirp_args = () if len(source_path) < 2 else source_path[:-1]
                unique_namespaces_in_source: Tuple[Tuple[str]] = tuple(sorted({ sym.ns for sym in source_syms }))
                for version_str in GameVersion.ALL_STR:
                    version = GameVersion.to_i(version_str)
                    pathlib.Path(os.path.join("output_hpp_sym_ns", version_str, *mkdirp_args)).mkdir(parents=True, exist_ok=True)
                    with open(os.path.join("output_hpp_sym_ns", version_str, *mkdirp_args, basename_output), "w") as hppfile:
                        hppfile.write("#pragma once\n\n")
                        hppfile.write("namespace sym {\n")

                        for ns in unique_namespaces_in_source:
                            hppfile.write("\n")
                            hppfile.write(''.join(f"namespace {ns_seg} {{ " for ns_seg in ns))

                            for sym in source_syms:
                                if sym.ns != ns:
                                    continue

                                address = sym.address[version]
                                if address == MagicWords.SKIP:
                                    continue

                                if address <= 0xffffffff:
                                    address = f"{address:#010x}"
                                else:
                                    address = f"{address:#018x}"

                                identifier = sym.subject_identifier or sym.mangled
                                hppfile.write(f"\n    static const u64 {identifier} = {address}; // {sym.mangled}")

                            hppfile.write("\n")
                            hppfile.write(' '.join("}" for ns_seg in ns))
                            hppfile.write("\n")
                        hppfile.write("\n} // sym\n")


if __name__ == "__main__":
    # TODO cleanup empty/stale files
    # rm -r output_hpp_sym_ns/TOTK_1*
    # rm output_ld_script/TOTK_*/syms_merged.ld

    run_symbol_def_imports()
    for v in GameVersion.ALL_STR:
        TrashCommands.build_gameversion_linkerscript_syms(GameVersion.to_i(v))
    TrashCommands.build_gameversion_hpp_syms()

