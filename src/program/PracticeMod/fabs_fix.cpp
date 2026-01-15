#ifdef TOTK_100
#include "exlaunch.hpp"
#include "misc.h"
// thanks dt: https://github.com/dt-12345/ZuggleAI/blob/master/source/program/fabs_fix.h
using namespace PracticeMod;

namespace PracticeMod::fabs_fix {
    HOOK_DEFINE_INLINE(FabsFix) {
        static void Callback(exl::hook::InlineCtx* ctx) {
            const PouchActorInfoRow* pai = reinterpret_cast<const PouchActorInfoRow*>(ctx->X[8]);
            if (!pai->mPouchStockable) {
                return;
            }

            auto getStructStruct = reinterpret_cast<GetStructStruct*>(exl::util::modules::GetTargetOffset(0x0081fed4));
            auto setStructEnum = reinterpret_cast<SetStructEnum*>(exl::util::modules::GetTargetOffset(0x00820bfc));
            auto setStructInt = reinterpret_cast<SetStructInt*>(exl::util::modules::GetTargetOffset(0x008202b0));
            auto calcHash = reinterpret_cast<CalcHash*>(exl::util::modules::GetTargetOffset(0x006feea8));
            auto s_GmdMgrPtr = reinterpret_cast<void**>(exl::util::modules::GetTargetOffset(0x04646c60));

            const StructHandle* slot_handle = reinterpret_cast<const StructHandle*>(ctx->X[2]);
            StructHandle effect_handle;
            if (!getStructStruct(*s_GmdMgrPtr, effect_handle, *slot_handle, 0xecff4f2a)) {
                return;
            }

            setStructEnum(*s_GmdMgrPtr, calcHash(pai->mpCureEffectType, 0), effect_handle, 0x746585e5);
            setStructInt(*s_GmdMgrPtr, pai->mCureEffectLevel, effect_handle, 0x195a67f5);
            setStructInt(*s_GmdMgrPtr, pai->mCureEffectiveTime < 1 ? 0 : pai->mCureEffectiveTime / 30, effect_handle, 0xaa1f82b8);
        }
    };

    void InstallHooks() {
        FabsFix::InstallAtOffset(0x00c95828);
    }
}
#endif

