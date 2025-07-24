#pragma once
#include <string>
#include <nn/hid.h>
#include "exlaunch.hpp"
#include "util/alloc.hpp"
using String = lotuskit::String;

namespace lotuskit::script::hotkey {
    class HotkeyBinding {
        public:
        u64 buttons; // bindType 1: nn::hid::NpadButtonSet flagset
                     // bindType 2: nn::hid::KeyboardKey u8[8]
        String arg; // local filename (execType 1, 2), AS source (execType 3)
        u8 bindType; // 0: none, 1: buttons, 2: keyboard
        u8 execType; // 0: none, 1: AS file, 2: nxtas file, 3: AS eval
        u8 bindCooldownRemaining; // naive callcount, reset to cooldownFrames on trigger
    };

    constexpr size_t MAX_HOTKEYS = 12;
    inline HotkeyBinding bindings[MAX_HOTKEYS] = {0}; // XXX defer alloc to mod init?
    void unassign(HotkeyBinding* hk);
    HotkeyBinding* getUnassigned();
    HotkeyBinding* getByButtons(u64 buttons);
    HotkeyBinding* getByKeyboardState(const nn::hid::KeyboardState& keyboard);
    HotkeyBinding* getByKeyboardTrigger(const String& trigger);
    u64 keyboardTriggerToButtons(const String& trigger);

    void calc(); // each frame: check for triggers + react
    void calcDispatch(HotkeyBinding* hk);
    void calcCooldown();

    void bindButtonsExecFile(u64 buttons, const String& filename);
    void bindButtonsExecFileNXTas(u64 buttons, const String& filename);
    void bindButtonsEval(u64 buttons, const String& scriptText);
    void unbindButtons(u64 buttons);

    void bindKeyboardExecFile(const String& trigger, const String& filename);
    void bindKeyboardExecFileNXTas(const String& trigger, const String& filename);
    void bindKeyboardEval(const String& trigger, const String& scriptText);
    void unbindKeyboard(const String& trigger);

    inline u8 cooldownFrames = 15;
    inline void setCooldown(u8 frames) { cooldownFrames = frames; }

} // ns

