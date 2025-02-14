#pragma once

namespace lotuskit::util::world {
    void InstallHooks();

    // TODO enum: 0 clear, 1 cloudy, 2 light rain, 3 heavy rain, 4 unbearable cold blizzard, 5 unbearable cold dark blizzard, 6 thunder, 7 thunder+rain, 8 blueskyrain, 9 caverain?, 10 snow, >=11 invalid/overcast?
    void setWeather(u32 weather, bool);

    void setFogDensity(float fogDensity, float fogNear, float fogFar, bool);

    inline bool doHackFogDisable = false;
    inline void doHackFogDisable_set(bool v) { doHackFogDisable = v; }

    void setTime(float time);
    void setTimeHM(float hours, float minutes=0);
    void setTimeDHMS(float days, float hours=0, float minutes=0, float seconds=0);

    void setWind(float azimuth, float elevation, float speed, bool isImmTrans=true);

} // ns

