#pragma once

namespace lotuskit::util::fps {
    void PrintStats();

    void wsAnnounceConfig();
    inline bool doTextWriter = false;
    inline void doTextWriter_set(bool v) { doTextWriter = v; wsAnnounceConfig(); }

} // ns

