## Getting Started
- Download the [latest build](https://github.com/aquacluck/totk-lotuskit/releases) for your game version. This is replaced with every commit, so you might want to hang on to the zip in case I break it in the future!
- Extract the zip and edit the configs in the `romfs` folder. Copy the `romfs` and `exefs` folders into your switch sd's `/atmosphere/contents/0100f2c0115b6000/` folder.
- Run `frontend/run.py` (you'll need a python installation) and open the web frontend at [http://127.0.0.1:7073](http://127.0.0.1:7073)
- Run the game. By default the mod will immediately connect to the frontend, but depending on the configs in the mod `romfs` folder it can also connect when you whistle, or it can remain offline (still logging to the debug console in Ryujinx).

## Building from source
- (Advanced) Manual preprocessing for certain changes:
    - Many headers are mirrored+patched from https://github.com/dt-12345/totk , see `dt_totk/CLOBBER_mirror_headers.sh` to update or further patch them. tldr it rewrites the folder with upstream + local patches, which we commit into this repo.
    - `syms/build.py` is a symbol map+metadata build pipeline, keep this in sync! The output files will get clobbered when you run this, but they're still easy to navigate, so feel free to just hack at the output in dev if you know you can get away with it. This system exposes a `sym` header include prefix holding `sym::` cpp namespaces organizing all your symbol access.
- Run `make` in a suitable build environment (such as [devkitPro](https://devkitpro.org/wiki/Getting_Started) or a [docker container](https://hub.docker.com/r/pixelkiri/devkitpro-alpine-switch/)) with cmake installed
- Mod `exefs` files are at `build/main.npdm` and `build/subsdk9`, copy `romfs` configs from the repo

## In-game Overlay
While most commands are issued through the web frontend, drawing within the game is useful for frame-perfect accuracy and similar bandwidth+latency constrained tasks. However this currently requires compatible font and shader files to be copied from another game, as TOTK supports but does not ship with these files. If any of these files are missing, the hooks supporting these features will not be installed and everything else should proceed as normal.

To enable these features, copy the below 8 romfs files from a compatible game such as Splatoon 3. Dumping romfs in Ryujinx is the simplest way to get to these for now.

```
~/appdata/Roaming/Ryujinx/sdcard$ sha256sum atmosphere/contents/0100f2c0115b6000/romfs/Lib/*/*/*
ba57e6d5368c09e510fb873f298148a1a9fc794a8d818d1578584cc876b9f75e  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font.ntx
b47ece0989036efc2c3ebdbce4765851c4d042859cec6e6505a15696d9a1084b  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font_jis1.ntx
93dd352bff4c8ee8020000c1c48e127ef801a5418c6c9200001b7e19dd2bf55d  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font_jis1_mipmap.xtx
63938de61a0acdaf6548ea97f32047d29edfb22121d2bff0cacbed70f1ab5b3d  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font_jis1_tbl.bin
ec580c761319e69a5b40a550b963eeb2450a71adbffaaf7abd0267b7dc174142  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font_shader.bin
ec24835b294116f40dabcdf7a167afa5c630f77b257da840fd445349cf598719  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font_shader_jis1.bin
2177ac377b82d6e96488ad0fc05f7ab91eb4a22ba480567269de4791b0231985  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/nvn_font/nvn_font_shader_jis1_mipmap.bin
dd9ec52b865c268faa648a459815ea7e30aca350b4be7945942693cc4e94f1d7  atmosphere/contents/0100f2c0115b6000/romfs/Lib/sead/primitive_renderer/primitive_drawer_nvn_shader.bin
```

## Credits & Thanks
- build + hooking + much more thanks to [exlaunch](https://github.com/shadowninja108/exlaunch) by shadow, licensed under GPL2
- build + Logger + InputHelper thanks to [LunaKit](https://github.com/Amethyst-szs/smo-lunakit) licensed under GPL2
- watertoon's type+symbol documentation, and so much more
- dt12345's type documentation, [actor-relation-tracker](https://github.com/dt-12345/actor-relation-tracker/), [sead::TextWriter examples](https://github.com/dt-12345/writer), and so much more
- lessons from inspecting MaxLastBreath's [UltraCam](https://github.com/MaxLastBreath/TOTK-mods/) binaries
- socket bits+pieces+reference from the [Atmosphere project](https://github.com/Atmosphere-NX/Atmosphere) licensed under GPL2
- socket+logging bits+pieces from the [Skyline project](https://github.com/skyline-dev/skyline) licensed under MIT
- [frontend font](https://github.com/adobe-fonts/source-code-pro) licensed under OFL
- [preact](https://github.com/preactjs/preact) licensed under MIT and [htm](https://github.com/developit/htm) licensed under Apache2.0, sourced from [esm builds](https://esm.sh)
- https://github.com/whitequark/python-itanium_demangler
- https://github.com/fruityloops1/zeldar

## LunaKit Credits
- [Amethyst-szs](https://github.com/Amethyst-szs): LunaKit Creator
- [CraftyBoss](https://github.com/CraftyBoss): SMO ExLaunch, ImGui, and more
- [Mars](https://github.com/Mars2032): Referenced TAS Mod, Emotional support
- [DaveGamble/cJSON](https://github.com/DaveGamble/cJSON): C JSON Parser
- [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/)
- [BDSP](https://github.com/Martmists-GH/BDSP)
- [Sanae](https://github.com/Sanae6)
- [oss-rtld](https://github.com/Thog/oss-rtld)
