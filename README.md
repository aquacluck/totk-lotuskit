## Getting started
- Download the [latest build](https://github.com/aquacluck/totk-lotuskit/releases) for your game version. This is replaced with every commit, so you might want to hang on to the zip in case I break it in the future!
- Extract the zip and edit the configs in the `romfs` folder. Install the mod by copying the `romfs` and `exefs` folders into your switch sd's `/atmosphere/contents/0100f2c0115b6000/` folder.
- Run the game.
- Depending on your settings in `romfs/totk_lotuskit/config.json`, the game may hang at bootup and wait for your frontend, or remain offline (still logging to the debug console in emulators), or ~~TODO you may connect later~~
- Run `frontend/run.py` (you'll need a python installation) and open the web frontend at [http://127.0.0.1:7073](http://127.0.0.1:7073)
- Connect to an emulator with localhost (default), or enter your switch console ip+port in the bottom right to connect to the mod.

## You should know
- **This is not a stable product!** Don't expect to leave the mod running during extended play without problems. Don't run the mod on your irreplaceable casual save file.
- Opening up a game/emulator process for arbitrary manipulation over the network is inherently risky, be sure you understand the risks and keep the device on safe networks.
- WebSocket connection to the mod must be established at bootup (for now) or not at all. There is no way to reconnect to a game. Yeah it sucks.
- Everything you do in the frontend is accomplished by running AngelScript snippets, and usually you can hover on buttons/etc for clarification about scripting or other quirks.
- Check out `registerGlobals()` in `src/program/script/globals.cpp` for a complete list of script bindings available!
- Polling options in the frontend are not perfectly reliable.
- Bug: Don't leave ActorWatchers targeting dead/dying actors while you go through loading screens (especially title+shrine loads), this will eventually crash. Run `ActorWatcher::clearSlot(i)` or target a resident actor like Player to avoid this.
- Bug: Spooky Recall red text
- The mod version must match your game version, be sure to switch them together. Separate builds simplify certain version interop problems like differing structs or register usage.
- By default the frontend is served on 7073 because it looks sort of like "TOTK". The mod's WebSocket server is on 7072 because it's like 2 is the sequel.
- TOTK versions 1.0.0 and 1.2.1 are primarily supported. I'll also build for 1.1.0, but the testing workload is too much while the project is in flux, so these builds are untested+unsupported for now.
- When troubleshooting be sure to check any emulator logs as well as ws traffic and console in browser devtools. Things like scripting syntax errors will show up there.
- When reporting mid-game freezing/crashing problems, I'll need a lot of detail! Console vs emulator, version, how long was the play session, where it happened, any recent loads or approaching any major landmarks, any crash dump/log, etc... Modding the game is very fragile, there are countless ways to fail, it's hard to narrow down and I'm not very good at C++ or reverse engineering :)

## Building the mod from source
- Manual preprocessing for certain work/changes:
    - Many headers are mirrored+patched from https://github.com/dt-12345/totk , see `dt_totk/CLOBBER_mirror.sh` to update or further patch them. tldr it rewrites the folder with upstream + local patches, which we commit into this repo.
    - angelscript source is similarly mirrored, and often receives nice new features
    - `syms/build.py` is a symbol map+metadata build pipeline, keep this in sync! A header file holding `sym::` prefixed symbols and a linker script symbol table for each game version will be generated.
- Run `make` in a suitable build environment (such as [devkitPro](https://devkitpro.org/wiki/Getting_Started) or a [docker container](https://hub.docker.com/r/pixelkiri/devkitpro-alpine-switch/)) with cmake installed
- Mod `exefs` files are at `build/main.npdm` and `build/subsdk9`, copy `romfs` configs from the repo

## Repo overview
- `.github` automatically builds and [releases](https://github.com/aquacluck/totk-lotuskit/releases) each commit. You could reproduce the build with this or set it up in your own repo clone.
- `angelscript` contains a patched mirror of the lib
- `dt_totk` contains many patched C++ headers for libs used in the game
- `frontend` is an optional web frontend copied into every release zip. A quickie python http server is provided, and you can serve this however you like, but non-http `file://` access will not work!
- `romfs` is a skeleton/template for files which should be installed with the mod, copied into every release zip
- `src/program` is the C++ mod source
- `syms` is a symbol map+metadata build system, see above for details. This isn't run during `make`, just done manually for now.

## Credits + Thanks
- [dt-12345](https://github.com/dt-12345/) for documentation+examples about so many aspects of this game's internals
- Watertoon's type+symbol+ainb documentation, and so much more
- build + hooking + much more thanks to [exlaunch](https://github.com/shadowninja108/exlaunch) by shadow, licensed under GPL2
- build + name thanks to [LunaKit](https://github.com/Amethyst-szs/smo-lunakit) by Amethyst, licensed under GPL2. Thanks [CraftyBoss](https://github.com/CraftyBoss), [Sanae](https://github.com/Sanae6), [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/), [oss-rtld](https://github.com/Thog/oss-rtld) for their contributions or references used in LunaKit.
- lessons from inspecting MaxLastBreath's [UltraCam](https://github.com/MaxLastBreath/TOTK-mods/) binaries
- socket bits+pieces+reference from the [Atmosphere project](https://github.com/Atmosphere-NX/Atmosphere) licensed under GPL2
- socket+logging bits+pieces from the [Skyline project](https://github.com/skyline-dev/skyline) licensed under MIT
- [AngelScript](https://www.angelcode.com/angelscript/) by Andreas Jönsson, under zlib license
- [frontend font](https://github.com/adobe-fonts/source-code-pro) licensed under OFL
- [preact](https://github.com/preactjs/preact) licensed under MIT and [htm](https://github.com/developit/htm) licensed under Apache2.0, sourced from [esm builds](https://esm.sh)
- https://github.com/whitequark/python-itanium_demangler
- https://github.com/fruityloops1/zeldar
- Nintendo for the good game
- and more

