#pragma once
// thanks dt: https://github.com/dt-12345/ZuggleAI/blob/master/source/program/fabs_fix.h
namespace PracticeMod::fabs_fix {
    #ifdef TOTK_100
        void InstallHooks();
    #else
        inline void InstallHooks() { } // nop
    #endif
}

