These patches are optional and loaded on request at runtime. They belong in `sdcard:/atmosphere/contents/0100f2c0115b6000/runtime_patches`. Example installation:
- Atmosphere: `sdcard:/atmosphere/contents/0100f2c0115b6000/runtime_patches/lotuskit-playercamera-unlock/082CE09B06E33A123CB1E2770F5F9147709033DB.ips`
- Windows Ryujinx: `%appdata%/Ryujinx/sdcard/atmosphere/contents/0100f2c0115b6000/runtime_patches/lotuskit-playercamera-unlock/082CE09B06E33A123CB1E2770F5F9147709033DB.ips`

You'll notice `.rips` (reverse IPS) files created here once lotuskit launches, these contain cached copies of vanilla code to quickly toggle patches. They're regenerated at bootup as needed and are safe to delete if you want.

