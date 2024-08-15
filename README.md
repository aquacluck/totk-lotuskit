## Getting Started
- Download the [latest build](https://github.com/aquacluck/totk-lotuskit/releases) for your game version. This is replaced with every commit, so you might want to hang on to the zip in case I break it in the future!
- Extract the zip and edit the configs in the `romfs` folder. Copy the `romfs` and `exefs` folders into your switch sd's `/atmosphere/contents/0100f2c0115b6000/` folder.
- Run `frontend/run.py` (you'll need a python installation) and open the web frontend at [http://127.0.0.1:7073](http://127.0.0.1:7073)
- Run the game. By default the mod will immediately connect to the frontend, but depending on the configs in the mod `romfs` folder it can also connect when you whistle, or it can remain offline (still logging to the debug console in Ryujinx).

## Building from source
- Run `make` in a suitable build environment (such as [devkitPro](https://devkitpro.org/wiki/Getting_Started) or a [docker container](https://hub.docker.com/r/pixelkiri/devkitpro-alpine-switch/)) with cmake installed
- Mod `exefs` files are at `build/main.npdm` and `build/subsdk9`, copy `romfs` configs from the repo

## Credits & Thanks
- build + hooking + much more thanks to [exlaunch](https://github.com/shadowninja108/exlaunch) by shadow, licensed under GPL2
- build + Logger + InputHelper thanks to [LunaKit](https://github.com/Amethyst-szs/smo-lunakit) licensed under GPL2
- watertoon's type+symbol documentation, and so much more
- dt12345's type documentation, [actor-relation-tracker](https://github.com/dt-12345/actor-relation-tracker/), and so much more
- lessons from inspecting MaxLastBreath's [UltraCam](https://github.com/MaxLastBreath/TOTK-mods/) binaries
- socket bits+pieces+reference from the [Atmosphere project](https://github.com/Atmosphere-NX/Atmosphere) licensed under GPL2
- socket+logging bits+pieces from the [Skyline project](https://github.com/skyline-dev/skyline) licensed under MIT
- [frontend font](https://github.com/adobe-fonts/source-code-pro) licensed under OFL
- [preact](https://github.com/preactjs/preact) licensed under MIT and [htm](https://github.com/developit/htm) licensed under Apache2.0, sourced from [esm builds](https://esm.sh)

## LunaKit Credits
- [Amethyst-szs](https://github.com/Amethyst-szs): LunaKit Creator
- [CraftyBoss](https://github.com/CraftyBoss): SMO ExLaunch, ImGui, and more
- [Mars](https://github.com/Mars2032): Referenced TAS Mod, Emotional support
- [DaveGamble/cJSON](https://github.com/DaveGamble/cJSON): C JSON Parser
- [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/)
- [BDSP](https://github.com/Martmists-GH/BDSP)
- [Sanae](https://github.com/Sanae6)
- [oss-rtld](https://github.com/Thog/oss-rtld)
