#pragma once

namespace lotuskit::util::fps {
    void PrintStats();

    inline u8 fixedFPS = 0;
    void fixedFPS_set(u8 v);

    void wsAnnounceConfig();
    inline bool doTextWriter = false;
    inline void doTextWriter_set(bool v) { doTextWriter = v; wsAnnounceConfig(); }

} // ns

