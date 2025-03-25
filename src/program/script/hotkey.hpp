#pragma once
#include <string>
#include "exlaunch.hpp"

namespace lotuskit::script::hotkey {
    class HotkeyBinding {
        public:
        u64 buttons;
        std::string arg; // local filename (1, 2), AS source (3)
        u8 bindType; // 0: none, 1: buttons
        u8 execType; // 0: none, 1: AS file, 2: nxtas file, 3: AS eval
        u8 bindCooldownRemaining; // naive callcount, reset to cooldownFrames on trigger
    };

    constexpr size_t MAX_HOTKEYS = 12;
    inline HotkeyBinding bindings[MAX_HOTKEYS] = {0};
    HotkeyBinding* getUnassigned();
    HotkeyBinding* getByButtons(u64 buttons);

    void calc(); // check for triggers + react
    void calcDispatch(HotkeyBinding* hk);
    void calcCooldown();

    void bindButtonsExecFile(u64 buttons, const std::string& filename);
    void bindButtonsExecFileNXTas(u64 buttons, const std::string& filename);
    void bindButtonsEval(u64 buttons, const std::string& scriptText);
    void unbindButtons(u64 buttons);

    inline u8 cooldownFrames = 15;
    inline void setCooldown(u8 frames) { cooldownFrames = frames; }

} // ns

