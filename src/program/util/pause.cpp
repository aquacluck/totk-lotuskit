#include <nn/util.h>
#include <cstring>

#include "exlaunch.hpp"
#include "util/hash.hpp"
#include "util/pause.hpp"
#include "TextWriter.hpp"

namespace lotuskit::util::pause {

    HOOK_DEFINE_TRAMPOLINE(PauseMgrInitializeHook) {
        static constexpr auto s_name = "engine::module::PauseMgr::initialize";
        static void Callback(engine::system::PauseMgr* self, void* heap) {
            char buf[200];
            nn::util::SNPrintf(buf, sizeof(buf), "PauseMgr::init(%p, %p)", self, heap);
            svcOutputDebugString(buf, strlen(buf));

            pauseMgr = self;
            Orig(self, heap);
        }
    };

    HOOK_DEFINE_INLINE(PauseMgrProcessRequestsHook3) {
        static constexpr auto s_name = "engine::module::PauseMgr::processRequests_inlineModuleCalc";
        static void Callback(exl::hook::InlineCtx* ctx) {
            if (isFreezeMask) {
                // overwrite pauseMgr->mContext.mPauseMask assignment operands
                ctx->W[11] = freezeMaskVal[0];
                ctx->W[10] = freezeMaskVal[1];
                ctx->W[ 9] = freezeMaskVal[2];
                ctx->W[ 8] = freezeMaskVal[3];
            }
        }
    };

    void InstallHooks() {
        PauseMgrInitializeHook::Install();
        //PauseMgrProcessRequestsHook::Install();
        //PauseMgrProcessRequestsHook2::InstallAtOffset(0x008ef280); // 121
        PauseMgrProcessRequestsHook3::Install();
    }

    void drawPauses() {
        if (!doTextWriter && !doTextWriterExtended) { return; }
        if (pauseMgr == nullptr) {
            lotuskit::TextWriter::printf(0, "PauseMgr(%p)\n\n", pauseMgr);
            return;
        }

        if (doTextWriterExtended) {
            auto& m = pauseMgr->mContext.mPauseMask;
            lotuskit::TextWriter::printf(0, "PauseMgr(%p).ctx.mask(%08x%08x%08x%08x)\n", pauseMgr, m[0], m[1], m[2], m[3]);

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


            if (isFreezeMask) {
                u32* mp = freezeMaskVal;
                lotuskit::TextWriter::printf(0, "freezeMask(%08x%08x%08x%08x)\n", *mp, *(mp+1), *(mp+2), *(mp+3));
            }

            for (const auto &value: engine::system::PauseHashes) {
                const auto &hash = value.first;
                const auto &name = value.second;
                const s32 count = pauseMgr->getPauseRequestCount(hash);
                if (count == 0) { continue; }
                //const char* name = pauseMgr->getPauseRequestKey(hash);
                u32* mp = pauseMgr->getPauseRequestMask(hash);
                lotuskit::TextWriter::printf(0, "PauseReq(%s, n=%d).mask(%08x%08x%08x%08x)\n", name, count, *mp, *(mp+1), *(mp+2), *(mp+3));
                //lotuskit::TextWriter::printf(0, "PauseReq(%s, n=%d).mask(%p%p) \n", name, count, *m, *(m+1));
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

    void doFreezeMask(bool b) {
        if (b) {
            std::memcpy(freezeMaskVal, pauseMgr->mContext.mPauseMask, 0x10);
        }
        isFreezeMask = b;
    }

    void freezeMask4x4(u32 v0, u32 v1, u32 v2, u32 v3) {
        freezeMaskVal[0] = v0;
        freezeMaskVal[1] = v1;
        freezeMaskVal[2] = v2;
        freezeMaskVal[3] = v3;
        isFreezeMask = true;
    }

    void freezeTarget(u32 targetHash, bool val, bool clearOthers) {
        u8 i = pauseMgr->getTargetIndexByHash(targetHash);
        freezeTargetIndex(i, val, clearOthers);
    }

    void freezeTargetStr(const std::string& targetKey, bool val, bool clearOthers) {
        u32 hash = lotuskit::util::hash::murmur32(targetKey);
        freezeTarget(hash, val, clearOthers);
    }

    void freezeTargetIndex(u8 targetIndex, bool val, bool clearOthers) {
        u32 v[4] = {0};
        if (!clearOthers) {
            std::memcpy(v, pauseMgr->mContext.mPauseMask, 0x10);
        }

        u32* mask = (u32*)((u64)(v) + ((targetIndex >> 3) & 0x1c));
        if (val) {
            *mask |= (1 << (targetIndex & 0x1f));
        } else {
            *mask &= ~(1 << (targetIndex & 0x1f));
        }

        std::memcpy(freezeMaskVal, v, 0x10);
        isFreezeMask = true;
    }

} // ns

