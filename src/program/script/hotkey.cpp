#include "script/hotkey.hpp"
#include "script/schedule.hpp"
#include "tas/Record.hpp"

namespace lotuskit::script::hotkey {

    void calc() {
        calcCooldown();
        const u64 buttons = *(u64*)(&lotuskit::tas::Record::currentInput.buttons);
        calcDispatch(getByButtons(buttons));
    }

    void calcCooldown() {
        for (size_t i=0; i < MAX_HOTKEYS; i++) {
            if (bindings[i].bindType != 0 && bindings[i].bindCooldownRemaining > 0) {
                bindings[i].bindCooldownRemaining--;
            }
        }
    }

    void calcDispatch(HotkeyBinding* hk) {
        if (hk == nullptr) { return; } // nop: no binding found
        if (hk->bindCooldownRemaining > 0) { return; } // nop: bind is on cooldown
        hk->bindCooldownRemaining = cooldownFrames; // begin cooldown
        if (hk->execType == 1) {
            lotuskit::script::schedule::tas::pushExecLocalFileModule(hk->arg, "void main()", false); // defer execution to main loop
        } else if (hk->execType == 2) {
            lotuskit::script::schedule::tas::pushExecLocalFileModuleNXTas(hk->arg, false); // defer execution to main loop
        } else if (hk->execType == 3) {
            lotuskit::script::schedule::tas::pushExecEval(hk->arg, "void main()", false); // defer execution to main loop
        } else { return; } // err: 0 or unknown type
    }

    HotkeyBinding* getUnassigned() {
        for (size_t i=0; i < MAX_HOTKEYS; i++) {
            if (bindings[i].bindType == 0) {
                return &bindings[i];
            }
        }
        return nullptr;
    }

    HotkeyBinding* getByButtons(u64 buttons) {
        for (size_t i=0; i < MAX_HOTKEYS; i++) {
            if (bindings[i].bindType == 1 && bindings[i].buttons == buttons) {
                return &bindings[i];
            }
        }
        return nullptr;
    }

    void bindButtonsExecFile(u64 buttons, const std::string& filename) {
        unbindButtons(buttons);
        auto hk = getUnassigned();
        if (hk == nullptr) { return; } // err: max hotkeys bound
        hk->buttons = buttons;
        hk->bindType = 1; // buttons
        hk->execType = 1; // AS file
        hk->arg = filename;
    }

    void bindButtonsExecFileNXTas(u64 buttons, const std::string& filename) {
        unbindButtons(buttons);
        auto hk = getUnassigned();
        if (hk == nullptr) { return; } // err: max hotkeys bound
        hk->buttons = buttons;
        hk->bindType = 1; // buttons
        hk->execType = 2; // nxtas file
        hk->arg = filename;
    }

    void bindButtonsEval(u64 buttons, const std::string& scriptText) {
        unbindButtons(buttons);
        auto hk = getUnassigned();
        if (hk == nullptr) { return; } // err: max hotkeys bound
        hk->buttons = buttons;
        hk->bindType = 1; // buttons
        hk->execType = 3; // AS eval
        hk->arg = scriptText;
    }

    void unbindButtons(u64 buttons) {
        auto hk = getByButtons(buttons);
        if (hk == nullptr) { return; } // nop: no binding found
        hk->buttons = 0;
        hk->bindType = 0;
        hk->bindCooldownRemaining = 0;
        hk->execType = 0;
        hk->arg = "";
    }

} // ns

