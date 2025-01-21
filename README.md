## Getting started
- Download the [latest build](https://github.com/aquacluck/totk-lotuskit/releases) for your game version. This is replaced with every commit, so you might want to hang on to the zip in case I break it in the future!
- Extract the zip and install the mod by copying the `romfs` and `exefs` folders into your switch sd's `/atmosphere/contents/0100f2c0115b6000/` folder.
- Run the game. ActorWatcher[0] should automatically be assigned to Player. Press **`L R ZL ZR + -`** together to start the ws server anytime.
- Run `frontend/run.py` (you'll need a python installation) and open the web frontend at [http://localhost:7073](http://localhost:7073)
- Connect to an emulator with localhost (default), or enter your switch console ip+port in the bottom right to connect to the mod. Emulators are easier to get started with.
- Fool around with things. You'll crash the game a lot and you'll be looking through some sort of code constantly, understand this going in and don't be frustrated :)

## You should know
- **This is not a stable product!** Don't expect to leave the mod running during extended play without problems. Don't run the mod on your irreplaceable casual save file.
- The mod version must match your game version, be sure to switch them together. Separate builds simplify certain version interop problems like differing structs or register usage. Don't worry about this for files under `frontend`, the frontend will handle all game versions with the same installation/assets.
- To update the mod, copy/drag the new `romfs` and `exefs` folders into their destination. It's always safe to overwrite with the new zip contents. Also serve the updated frontend, refresh it in your browser if needed, etc.
- TOTK versions 1.0.0 and 1.2.1 are primarily supported. I'll also build for 1.1.0, but the testing workload is too much while the project is in flux, so these builds are untested+unsupported for now.
- Everything you do in the frontend is accomplished by running AngelScript snippets, and usually you can hover on buttons/etc for clarification about scripting or other quirks.
- Check the "AS📖" button in the bottom right for script docs, see `registerGlobals` in `src/program/script/globals.cpp` for a complete list of script bindings available, and see [AngelScript's docs](https://www.angelcode.com/angelscript/sdk/docs/manual/doc_script.html) for language details (it's mostly "interpreted C++" with `@handles` instead of `*pointers`)
- By default the frontend is served on 7073 because it looks sort of like "TOTK". The mod's WebSocket server is on 7072 because it's like 2 is the sequel.

## Known bugs, sharp edges, and bad times
- When troubleshooting be sure to check any emulator logs as well as ws traffic and console in browser devtools. Things like scripting syntax errors will show up there.
- Opening up a game/emulator process for arbitrary manipulation over the network is inherently risky, be sure you understand the risks and keep the device on safe networks.
- Don't leave ActorWatchers targeting dead/dying actors while you go through loading screens (especially title+shrine loads), this will eventually crash. Clear the watcher or target a resident actor like Player to avoid this.
- Only 1 WebSocket connection to the mod per launch. There is no way to disconnect+reconnect to the game yet.
- Polling options in the frontend aren't perfect -- if the browser tries to connect right as the mod is opening its socket, the frontend can get stuck unable to connect.
- Actors may fail to spawn in some scripts with ifs/branches? or matrix constants unless assigned to an intermediate?
- Script size is limited to ~8KB and crashes on overflow
- camera "above" button may choose bad angles resulting in inverted controls. This state is prone to crashing when taking out devices + more
- PrimitiveDrawer shapes may be stale or repeated/overlapping when drawn after a pause, because tools like ActorWatcher try to draw the entire time without clearing the FrameHeap
- The mod can't bind+serve on ipv6 yet -- the modded device must be accessed with a `ws://192.168.0.69:7072` style 4 number ipv4 address. This should only be a problem for ipv6-only LANs (somewhat exotic for home networks)
- When reporting mid-game freezing/crashing problems, I'll need a lot of detail! Console vs emulator, version, how long was the play session, where it happened, any recent loads or approaching any major landmarks, any crash dump/log, etc... Modding the game is very fragile, there are countless ways to fail, it's hard to narrow down and I'm not very good at C++ or reverse engineering :)

## Building the mod from source
- Manual preprocessing for certain work/changes:
    - Many headers are mirrored+patched from https://github.com/dt-12345/totk , see `dt_totk/CLOBBER_mirror.sh` to update or further patch them. tldr it rewrites the folder with upstream + local patches, which we commit into this repo.
    - angelscript source is similarly mirrored, and often receives nice new features
    - `syms/build.py` is a symbol map+metadata build pipeline, keep this in sync! A header include file for each game version holding symbols prepared for exlaunch's lookup utilities will be generated. Symbols supplied mangled will also be included in the linker script.
- Run `make` in a suitable build environment (such as [devkitPro](https://devkitpro.org/wiki/Getting_Started) or a [docker container](https://hub.docker.com/r/devkitpro/devkita64)) with cmake installed
- Mod `exefs` files are at `build/main.npdm` and `build/subsdk9`, copy `romfs` configs from the repo

## Repo overview
- `.github` automatically builds and [releases](https://github.com/aquacluck/totk-lotuskit/releases) each commit. You could reproduce the build with this or set it up in your own repo clone.
- `angelscript` contains a patched mirror of the lib
- `dt_totk` contains many patched C++ headers for libs used in the game
- `exlaunch` contains a patched mirror
- `frontend` is an optional web frontend copied into every release zip. A quickie python http server is provided, and you can serve this however you like, but non-http `file://` access will not work!
- `romfs` is a skeleton/template for files which should be installed with the mod, copied into every release zip
- `src/program` is the C++ mod source
- `syms` is a symbol map+metadata build system, see above for details. This isn't run during `make`, just done manually for now.

## Credits + Thanks
- [dt-12345](https://github.com/dt-12345/) for documentation+examples about so many aspects of this game's internals
- Watertoon's type+symbol+ainb documentation, and so much more
- build + hooking + much more thanks to [exlaunch](https://github.com/shadowninja108/exlaunch) by shadow, licensed under GPL2
- build + name thanks to [LunaKit](https://github.com/Amethyst-szs/smo-lunakit) by Amethyst, licensed under GPL2. Thanks [CraftyBoss](https://github.com/CraftyBoss), [Sanae](https://github.com/Sanae6), [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/), [oss-rtld](https://github.com/Thog/oss-rtld) for their contributions or references used in LunaKit.
- Phil + TOTK Data Collection community for: actor list, recall+camera+lotus icons
- lessons from inspecting MaxLastBreath's [UltraCam](https://github.com/MaxLastBreath/TOTK-mods/) binaries
- influences from the [baritone](https://github.com/cabaletta/baritone) and [STROOP](https://github.com/SM64-TAS-ABC/STROOP) projects
- socket bits+pieces+reference from the [Atmosphere project](https://github.com/Atmosphere-NX/Atmosphere) licensed under GPL2
- socket+logging bits+pieces from the [Skyline project](https://github.com/skyline-dev/skyline) licensed under MIT
- [AngelScript](https://www.angelcode.com/angelscript/) by Andreas Jönsson, under zlib license
- [frontend font](https://github.com/adobe-fonts/source-code-pro) licensed under OFL
- [preact](https://github.com/preactjs/preact) licensed under MIT and [htm](https://github.com/developit/htm) licensed under Apache2.0, sourced from [esm builds](https://esm.sh)
- [fuzzysort](https://github.com/farzher/fuzzysort) licensed under MIT
- https://github.com/whitequark/python-itanium_demangler
- https://github.com/fruityloops1/zeldar
- Nintendo for the good game
- and more

