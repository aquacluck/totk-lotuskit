// fog hacks
world::doHackFogDisable(true); // zeldar's fog hack (causes graphical artifacts)
world::setFogDensity(0, 0, 0); // density, near, far (floats, confusing behavior)

// time hacks
world::setTime(60*60*4 + 60*20); // set time in seconds (float)
world::setTimeHM(4, 20);
world::setTimeDHMS(365, 4, 20, 69); // idk if this does anything

// weather hacks
world::setWeather(0); // clear

// TODO enum
// 0 clear?
// 1 cloudy
// 2 light rain
// 3 heavy rain
// 4 unbearable cold blizzard
// 5 unbearable cold dark blizzard
// 6 thunder
// 7 thunder+rain
// 8 blueskyrain
// 9 caverain?
// 10 snow
// >=11 invalid/overcast?

float azimuth = 0; // 0 rad south, pi/2 east, -pi/2 west, +-pi north
float elevation = 0; // level at 0 rad, up pi/2, down -pi/2
float speed = 10; // m/s? idk
world::setWind(azimuth, elevation, speed);

