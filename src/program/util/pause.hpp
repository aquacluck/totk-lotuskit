#pragma once
#include <string>
#include "structs/engineSystemPause.hpp"

namespace lotuskit::util::pause {
    void InstallHooks();
    void drawPauses();

    inline bool doTextWriter = false;
    inline void doTextWriter_set(bool v) { doTextWriter = v; }
    inline bool doTextWriterExtended = false;
    inline void doTextWriterExtended_set(bool v) { doTextWriterExtended = v; }
    inline engine::system::PauseMgr* pauseMgr = nullptr;

    void requestPause(u32 reqHash);
    void requestPauseStr(const std::string&);
    void releasePause(u32 reqHash);
    void releasePauseStr(const std::string&);

    inline bool isFreezeMask = false;
    inline u32 freezeMaskVal[4] = {0};
    void doFreezeMask(bool);
    void freezeMask4x4(u32 v0, u32 v1, u32 v2, u32 v3);
    void freezeTarget(u32 targetHash, bool val=true, bool clearOthers=true);
    void freezeTargetStr(const std::string&, bool val=true, bool clearOthers=true);
    void freezeTargetIndex(u8 targetIndex, bool val=true, bool clearOthers=true);

} // ns

