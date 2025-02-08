#pragma once
#include "structs/engineSystemPause.hpp"

namespace lotuskit::util::pause {
    void InstallHooks();
    void drawPauses();

    inline bool doTextWriter = false;
    inline void doTextWriter_set(bool v) { doTextWriter = v; }
    inline bool doTextWriterExtended = false;
    inline void doTextWriterExtended_set(bool v) { doTextWriterExtended = v; }
    inline engine::system::PauseMgr* pauseMgr = nullptr;

} // ns

