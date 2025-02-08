#include <nn/util.h>
#include <cstring>

#include "exlaunch.hpp"
#include "util/hash.hpp"
#include "util/pause.hpp"
#include "TextWriter.hpp"

namespace lotuskit::util::pause {

    HOOK_DEFINE_TRAMPOLINE(PauseMgrInitializeHook) {
        static constexpr auto s_name = "engine::system::PauseMgr::initialize"; // hacks
        static void Callback(engine::system::PauseMgr* self, void* heap) {

            char buf[200];
            nn::util::SNPrintf(buf, sizeof(buf), "PauseMgr::init(%p, %p)", self, heap);
            svcOutputDebugString(buf, strlen(buf));

            pauseMgr = self;
            Orig(self, heap);
        }
    };

    void InstallHooks() {
        PauseMgrInitializeHook::Install();
    }

    void drawPauses() {
        if (!doTextWriter && !doTextWriterExtended) { return; }
        if (pauseMgr == nullptr) {
            lotuskit::TextWriter::printf(0, "PauseMgr(%p)\n\n", pauseMgr);
            return;
        }

        if (doTextWriterExtended) {
            u8** m = (u8**)(pauseMgr->mContext.mPauseMask);
            lotuskit::TextWriter::printf(0, "PauseMgr(%p).ctx.mask(%p%p)\n", pauseMgr, *m, *(m+1));

            char mstr[200]; s32 mstr_i = 0; u8 mstr_col = 0;
            for (const auto &value: engine::system::PauseTargetHashes) {
                const auto &hash = value.first;
                const auto &name = value.second;
                if (!pauseMgr->isTargetPaused(hash)) { continue; }
                //u8 ti = pauseMgr->getTargetIndexByHash(hash);

                auto len = strlen(name);
                std::memcpy(&mstr[mstr_i], name, len);
                mstr_i += len;
                mstr_col += len;
                if (mstr_col >= 109) {
                    mstr[mstr_i++] = '|';
                    mstr[mstr_i++] = '\n';
                    mstr[mstr_i++] = '\0';
                    lotuskit::TextWriter::printf(0, mstr);
                    mstr_col = 0; // wrap line
                    mstr_i = 0;
                } else {
                    mstr_col++;
                    mstr[mstr_i++] = '|';
                }
            }
            if (mstr_i > 2) {
                mstr[mstr_i-1] = '\n';
                mstr[mstr_i  ] = '\0';
                lotuskit::TextWriter::printf(0, mstr);
            }
            lotuskit::TextWriter::printf(0, "\n");

            for (const auto &value: engine::system::PauseHashes) {
                const auto &hash = value.first;
                const auto &name = value.second;
                const s32 count = pauseMgr->getPauseRequestCount(hash);
                if (count == 0) { continue; }
                //const char* name = pauseMgr->getPauseRequestKey(hash);
                m = (u8**)(pauseMgr->getPauseRequestMask(hash));
                lotuskit::TextWriter::printf(0, "PauseReq(%s, n=%d).mask(%p%p) \n", name, count, *m, *(m+1));
            }
            lotuskit::TextWriter::printf(0, "\n");

        } else if (doTextWriter) {
            lotuskit::TextWriter::printf(0, "PauseMgr(%p)\n", pauseMgr);
            for (const auto &value: engine::system::PauseHashes) {
                const auto &hash = value.first;
                const auto &name = value.second;
                const s32 count = pauseMgr->getPauseRequestCount(hash);
                if (count == 0) { continue; }
                //const char* name = pauseMgr->getPauseRequestKey(hash);
                lotuskit::TextWriter::printf(0, "PauseReq(%s, n=%d)\n", name, count);
            }
            lotuskit::TextWriter::printf(0, "\n");
        }
    }

    void requestPause(u32 reqHash) {
        using impl_t = void* (u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("engine::module::PauseMgr::requestPause");
        impl(reqHash);
    }

    void requestPauseStr(const std::string& reqKey) {
        u32 hash = lotuskit::util::hash::murmur32(reqKey);
        requestPause(hash);
    }

    void releasePause(u32 reqHash) {
        using impl_t = void* (u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("engine::module::PauseMgr::releasePause");
        impl(reqHash);
    }

    void releasePauseStr(const std::string& reqKey) {
        u32 hash = lotuskit::util::hash::murmur32(reqKey);
        releasePause(hash);
    }

} // ns

