#pragma once

#include <container/seadBuffer.h>

namespace engine::system {

    // TODO extract to exkingEnums/similar?
    // TODO generally only store strings we actually need in the binary (frontend can handle the full dataset)
    constexpr std::pair<u32, const char*> PauseHashes[22] = {
        {0x0c05d321, "ResetGimmickFromScript"},
        {0x0eafe200, "LoadingPause"},
        {0x19f4eaec, "UpdateBancResource"},
        {0x2d67e2da, "DebugPause"},
        {0x33d6ecce, "SceneBeginning"},
        {0x4a02340c, "EventPlayMovie"},
        {0x5cf23aab, "EventDisableController_Camera"},
        {0x6a5bc18e, "EventSetupOrRestore"},
        {0x90ddf88b, "PostGamePause"},
        {0x9310c8ed, "GamePauseWithoutEvent"},
        {0x956925e5, "SystemPauseWhileLoading"},
        {0x95929b25, "UpdateResourcePause"},
        {0x95dfb2f7, "EventBancChangePause"},
        {0xa3895d9f, "ResetGimmick"},
        {0xa74d09ca, "AutoBuilderPause"},
        {0xb584333e, "PreGamePause"},
        {0xb5b0a658, "GameOverPause"},
        {0xba597383, "GamePause"},
        {0xd468c7d1, "EventDisableController"},
        {0xec94b0f4, "SceneTransition"},
        {0xfbad2d6a, "ClickPause"},
        {0xfebc8d2a, "ReverseRecorderPause"},
    };

    constexpr std::pair<u32, const char*> PauseTargetHashes[87] = {
        {0x3374793b, "AI"},
        {0x933bb020, "AS"},
        {0x9610d708, "Actor"},
        {0x61b94644, "Blackboard"},
        {0x7f771e39, "CSharp"},
        {0x06c42abf, "Camera"},
        {0x0787fb66, "Controller"},
        {0xe87cdf6a, "Custom_ASGlobalFrame"},
        {0x7bf055fc, "Custom_ActorCalcDrawExceptPouchActor"},
        {0xe2c8d2ec, "Custom_ActorCalcExceptAutoBuilderActor"},
        {0x0dd5a812, "Custom_ActorCalcExceptFarDelete"},
        {0xeb7181da, "Custom_ActorCalcExceptOpeningEventActor"},
        {0x0778e566, "Custom_ActorCalcExceptPlayerEquipment"},
        {0xb95e7d6d, "Custom_ActorCalcExceptPlayer"},
        {0x9f0b75e7, "Custom_ActorCalc"},
        {0xfd709f05, "Custom_ActorDraw"},
        {0x9f16529f, "Custom_ActorRenderingProxy"},
        {0x4c9c2c61, "Custom_AllowCalcScript"},
        {0x678e114b, "Custom_AllowStartScript"},
        {0xaa06420e, "Custom_AmiiboMgr"},
        {0x8712521b, "Custom_AttentionSystem"},
        {0x786f777e, "Custom_AutoPlacementMgr"},
        {0x4bff4095, "Custom_AutoSaveCount"},
        {0x7a3ce632, "Custom_AutoSave"},
        {0xf42e425a, "Custom_BancLoadMgrCheckGameData"},
        {0x5630a217, "Custom_BancRealtimeEdit"},
        {0x7dcdf858, "Custom_ChallengeFollowerMgr"},
        {0x935b28ed, "Custom_Chemical"},
        {0x3d7a92b9, "Custom_CompanionMgrSummon"},
        {0x8c71f099, "Custom_Controller_Camera"},
        {0x53920242, "Custom_Controller_Player"},
        {0xdf3e748d, "Custom_Controller_UI"},
        {0x8d0ff7b9, "Custom_CraftSignboardMgr"},
        {0x383dad75, "Custom_DragonMgr"},
        {0xbda67d57, "Custom_DynamicResolution"},
        {0xe9265f58, "Custom_EffectAutoBuild"},
        {0x7f2e4a04, "Custom_EffectReverseRecorder"},
        {0x5674d232, "Custom_EffectSystem"},
        {0x6e261364, "Custom_EnergyGaugeMgr"},
        {0xf0c7f516, "Custom_Footprint"},
        {0x542fb941, "Custom_ForbidLoadingPause"},
        {0xefcd53e4, "Custom_GSysAndController"},
        {0xe57eb13e, "Custom_GSysPauseMenu"},
        {0x278b1c12, "Custom_GSys"},
        {0x95685368, "Custom_GameBalance"},
        {0x7c76c22b, "Custom_GameEffect"},
        {0x8a98369f, "Custom_LaunchParameterMgr"},
        {0xf2ab66d9, "Custom_LightBallBudMgr"},
        {0xa2c8c520, "Custom_Logic"},
        {0xfcf2f0fa, "Custom_NpcPhysicsProcOnReset"},
        {0xbc431672, "Custom_PerceptionCalc"},
        {0x51897502, "Custom_PhysicsUpdateWorld"},
        {0xef3056f1, "Custom_PlayReportCount"},
        {0xf6e0a4c8, "Custom_Player"},
        {0x4fe69c14, "Custom_ReverseRecorder"},
        {0x0b8f0d5e, "Custom_SceneDraw"},
        {0xb18ee279, "Custom_Sequence"},
        {0x658af758, "Custom_SkipDamageCalc"},
        {0x16975da5, "Custom_SoundBgm"},
        {0xc7312b9c, "Custom_SoundEnv"},
        {0x3fae7675, "Custom_SoundPauseAll"},
        {0x895633f4, "Custom_SoundWorldExceptEnv"},
        {0x262e9f13, "Custom_SpecialPowerFilter"},
        {0x560a37d9, "Custom_StopCotrollerRumble"},
        {0xce9622ba, "Custom_StopSaveLiveToGameData"},
        {0xa846d3bb, "Custom_TimerMgr"},
        {0x5e415429, "Custom_UpdateBancResource"},
        {0x180e8308, "Custom_WaitLoadForUI"},
        {0xc67e547e, "Custom_WorldMgrImm"},
        {0x5f00fb18, "Custom_WorldMgr"},
        {0x5a3159a7, "Dengu"},
        {0xec816269, "Ecpp"},
        {0xecff4f2a, "Effect"},
        {0xa9956e41, "Error"},
        {0x98b5a07e, "Event"},
        {0xfa985875, "GameData"},
        {0x130efc1f, "Graphics"},
        {0xd2c6f0e9, "LOD"},
        {0xc42c5d7d, "ProgramHotReload"},
        {0x0d2c616e, "Reaction"},
        {0x5ba4bc44, "Resource"},
        {0xb7a94c8f, "Rumble"},
        {0x5bac3070, "Scene"},
        {0x83e025b8, "Tool"},
        {0x41a318b6, "UI"},
        {0x0a7b6093, "Visualize"},
        {0xdec6613c, "XLink"},
    };

    class Pause {
        public:
        char* mKey;
        u32 mHash;
        u32 mPauseMask[4];
        u32 mPauseCount; // XXX atomic?
    };

    inline s32 pauseCmpHashFn(const Pause& lhs, const u32& rhs_hash) {
        if (lhs.mHash == rhs_hash) { return  0; }
        if (lhs.mHash <  rhs_hash) { return -1; }
                              else { return  1; }
    };

    class PauseTargetArray {
        public:
        u32* mHashArray;
        u8* mIndexArray;
        s32 mCount;
        s32 mCapacity;
    };

    class PauseContext {
        public:
        u32 mPauseMask[4];
        PauseTargetArray* mTargetArrayPtr;
    };

    class PauseMgr {
        public:
        sead::Buffer<Pause> mRequestArray;
        PauseTargetArray mTargetArray;
        void* mPauseConfigDocument;
        PauseContext mContext;

        void** mUnk0x48;  // vtable with a bunch of stubbed functions
        u8 mUnk0x50[0x10];

        inline s32 getPauseRequestIndex(u32 pauseHash) {
            return mRequestArray.binarySearch<u32>(pauseHash, pauseCmpHashFn);
        }

        inline s32 getPauseRequestCount(u32 pauseHash) {
            if (mRequestArray.getSize() == 0) { return 0; }
            const s32 index = getPauseRequestIndex(pauseHash);
            return mRequestArray[index].mPauseCount;
        }

        inline u32* getPauseRequestMask(u32 pauseHash) {
            if (mRequestArray.getSize() == 0) { return 0; }
            const s32 index = getPauseRequestIndex(pauseHash);
            return mRequestArray[index].mPauseMask;
        }

        /*
        inline char* getPauseRequestKey(u32 pauseHash) {
            if (mRequestArray.getSize() == 0) { return 0; }
            const s32 index = getPauseRequestIndex(pauseHash);
            return mRequestArray[index].mKey;
        }
        */

        inline bool isTargetPaused(u32 pauseTargetHash) {
            const s32 cap = mTargetArray.mCapacity;
            if (cap == 0) { return false; }

            for (s32 i = pauseTargetHash % cap; i < cap; i++) {
                if (mTargetArray.mHashArray[i] != pauseTargetHash) {
                    continue; // skip through collisions
                }

                const u8 index = mTargetArray.mIndexArray[i];
                const u32 mask = *(u32*)((u64)(mContext.mPauseMask) + ((index >> 3) & 0x1c));
                return ((mask >> (index & 0x1f)) & 1) != 0;
            }

            return false;
        }

        inline u8 getTargetIndexByHash(u32 pauseTargetHash) {
            const s32 cap = mTargetArray.mCapacity;
            if (cap == 0) { return 0; } // XXX not found

            for (s32 i = pauseTargetHash % cap; i < cap; i++) {
                if (mTargetArray.mHashArray[i] != pauseTargetHash) {
                    continue; // skip through collisions
                }

                const u8 index = mTargetArray.mIndexArray[i];
                return index; // found
            }

            return 0; // XXX not found
        }

    };
    static_assert(sizeof(PauseMgr) == 0x60);

} // ns

