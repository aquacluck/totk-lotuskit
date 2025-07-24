#pragma once
#include "structs/engineSystemPause.hpp"
#include "util/alloc.hpp"
using String = lotuskit::String;

namespace lotuskit::util::pause {
    void InstallHooks();
    void drawPauses();
    inline engine::system::PauseMgr* pauseMgr = nullptr;

    void requestPause(u32 reqHash);
    void requestPauseStr(const String&);
    void releasePause(u32 reqHash);
    void releasePauseStr(const String&);

    inline bool isFreezeMask = false;
    inline u32 freezeMaskVal[4] = {0};
    void doFreezeMask(bool);
    void freezeMask4x4(u32 v0, u32 v1, u32 v2, u32 v3);
    void freezeRequest(u32 reqHash, bool value=false, bool clearOthers=true);
    void freezeRequestStr(const String&, bool value=false, bool clearOthers=true);
    void freezeTarget(u32 targetHash, bool val=true, bool clearOthers=true);
    void freezeTargetStr(const String&, bool val=true, bool clearOthers=true);
    void freezeTargetIndex(u8 targetIndex, bool val=true, bool clearOthers=true);

    bool isPauseRequest(u32);
    bool isPauseRequestStr(const String&);
    bool isPauseTarget(u32);
    bool isPauseTargetStr(const String&);

    void processFrozen(); // reapply freezeMaskVal to context mask
    void wsAnnounceConfig();

    inline bool doTextWriter = false;
    inline void doTextWriter_set(bool v) { doTextWriter = v; wsAnnounceConfig(); }
    inline bool doTextWriterExtended = false;
    inline void doTextWriterExtended_set(bool v) { doTextWriterExtended = v; wsAnnounceConfig(); }

} // ns

