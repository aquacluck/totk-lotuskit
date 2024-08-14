## Building
- edit `CMakeLists.txt` to select your game version, for now only `TOTK_100` or `TOTK_121` (yes you need cmake)
- edit `frontend/run.py` and update `BackendSocketConfig` -- this should be your PC's LAN address which the switch will connect to. Use localhost for emulators
- run `make` in a suitable build environment (such as [devkitPro](https://devkitpro.org/wiki/Getting_Started) [(docker)](https://hub.docker.com/u/devkitpro/) )
- copy the output files `build/main.npdm` and `build/subsdk9` into your target's `/atmosphere/contents/0100f2c0115b6000/exefs/` folder
- copy the `romfs` folder to your target's `/atmosphere/contents/0100f2c0115b6000/romfs/` folder and edit the config
- run the python, load the web ui, launch the game. whistle to retry the mod<-\>python connection

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
