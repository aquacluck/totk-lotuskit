#pragma once
#include "lib.hpp"
#include "lib/util/ptr_path.hpp"
#include "nn/hid.h"

#include "helpers/TASHelper.hpp"
#include "sym/engine/hacks.h"

namespace lotuskit::hooks::tas {
    //constexpr u32 TICKS_PER_SECOND = 19200000;

/*
HOOK_DEFINE_INLINE(MainGetNpadStates) {
    static const ptrdiff_t s_offset = 0x02a26834;

    static void Callback(exl::hook::InlineCtx* ctx) {
        Logger::main->logf(NS_DEFAULT_TEXT, R"({"uhh": "state %p , while_i %d , uVar9 %d , uVar13 %d , param_1 %p"})", ctx->X[0], ctx->W[24], ctx->W[8], ctx->W[13], ctx->X[19] );
    }
};
*/

HOOK_DEFINE_TRAMPOLINE(MainGetNpadStates) {
    static const ptrdiff_t s_offset = sym::engine::MainGetNpadStates; // hacks

    static void Callback(void* param_1) {
        // TODO option to lock/snap inputs to 90d/45d while ZL is held?
        // TODO see if we can totally stub this. bad sampling number or something if we don't call it, appears to recreate controller stuff every frame `ServiceAm .ctor: Applet 'Controller' created`
        // HLE.OsThread.47 ServiceHid Start: Stubbed. ControllerApplet ArgPriv 20 1072 ShowControllerSupport HoldType:Horizontal StyleSets:ProController, Handheld, JoyconPair
        Orig(param_1);

        constexpr u32 target_idk = 0; // styleset? iterates over 3?
        nn::hid::NpadBaseState* state = (nn::hid::NpadBaseState*)(param_1 + target_idk * 0xe98 + 0x58);
        auto tick = svcGetSystemTick();
        Logger::main->logf(NS_TAS, R"({"state": "%p", "sample": %d, "tick": %d, "LDown": "%d %d"})", state, state->mSamplingNumber, tick, state->mAnalogStickL.mY, state->mButtons);

        // playback
        if (lotuskit::tas::TASHelper::do_playback_simple || lotuskit::tas::TASHelper::do_playback_file) {
            auto input_element = lotuskit::tas::TASHelper::playback_current.load();
            if (input_element != nullptr) {
                input_element->apply(state);
            }
        }

        /*
        if (InputHelper::isKeyHold(nn::hid::KeyboardKey::Backquote)) {
            state->mAnalogStickL.mX = 0;
            state->mAnalogStickL.mY = -32000;
        } else {
            state->mAnalogStickL.mX = 0;
            state->mAnalogStickL.mY = 0;
        }
        */
    }
};

/*
typedef nn::util::BitFlagSet<64, NpadButton> NpadButtonSet;
struct AnalogStickState {
    s32 mX;
    s32 mY;
};
struct NpadBaseState {
    u64 mSamplingNumber;
    NpadButtonSet mButtons;
    AnalogStickState mAnalogStickL;
    AnalogStickState mAnalogStickR;
    NpadAttributeSet mAttributes;
    char reserved[4];
};
*/

}
