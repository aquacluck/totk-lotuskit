#include "script/hotkey.hpp"
#include "script/schedule.hpp"
#include "tas/Record.hpp"

namespace lotuskit::script::hotkey {

    void calc() {
        calcCooldown();

        const u64 buttons = *(u64*)(&lotuskit::tas::Record::currentInput.buttons);
        calcDispatch(getByButtons(buttons));

        nn::hid::KeyboardState keyboard;
        nn::hid::GetKeyboardState(&keyboard);
        //void* dbg_mods = &keyboard.mModifiers._storage;
        //void* dbg_keys = &keyboard.mKeys._storage;
        //lotuskit::HexDump::textwriter_printf_u8(0, dbg_mods, (u8*)dbg_mods, 1, true); // 32b, first 4 bytes here
        //lotuskit::HexDump::textwriter_printf_u8(0, dbg_keys, (u8*)dbg_keys, 2, true); // 256b, both full lines here
        calcDispatch(getByKeyboardState(keyboard));
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
            lotuskit::script::schedule::tas::pushExecLocalFileModule(hk->arg, "void main()", false, false); // defer execution to main loop, noblock on DebugPause
        } else if (hk->execType == 2) {
            lotuskit::script::schedule::tas::pushExecLocalFileModuleNXTas(hk->arg, false, false); // defer execution to main loop, noblock on DebugPause
        } else if (hk->execType == 3) {
            lotuskit::script::schedule::tas::pushExecEval(hk->arg, "void main()", false, false); // defer execution to main loop, noblock on DebugPause
        } else { return; } // err: 0 or unknown type
    }

    void unassign(HotkeyBinding* hk) {
        if (hk == nullptr) { return; }
        hk->buttons = 0;
        hk->bindType = 0;
        hk->bindCooldownRemaining = 0;
        hk->execType = 0;
        hk->arg = "";
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

    HotkeyBinding* getByKeyboardState(const nn::hid::KeyboardState& keyboard) {
        for (size_t i=0; i < MAX_HOTKEYS; i++) {
            if (bindings[i].bindType != 2 || bindings[i].buttons == 0) { continue; }
            u8* trigBit = (u8*)&(bindings[i].buttons);

            bool isContinue = false;
            for (size_t j=0; j < 8; j++) {
                if (*trigBit == 0) { break; } // ok: no more bits in trigger to check, full match
                                              // (buttons == 0 would always match here, so its skipped above)

                // is bit index *trigBit set? ty lunakit https://github.com/Amethyst-szs/smo-lunakit/blob/5d97fc6204c58b6a40557b455788d76cdcd0df82/src/nn/util.h#L86
                const auto storageBits = nn::util::BitFlagSet<256, nn::hid::KeyboardKey>::StorageBitCount;
                const u64 stor = keyboard.mKeys._storage[static_cast<u64>(*trigBit) / storageBits];
                if ((stor & (1ull << static_cast<u64>(*trigBit) % storageBits)) == 0) {
                    // no match, continue to next i iter
                    isContinue = true;
                    break;
                }

                trigBit++; // partial match, continue checking trigger bits
            }
            if (isContinue) { continue; }
            return &bindings[i];
        }
        return nullptr;
    }

    HotkeyBinding* getByKeyboardTrigger(const std::string& trigger) {
        u64 buttons = keyboardTriggerToButtons(trigger);
        for (size_t i=0; i < MAX_HOTKEYS; i++) {
            if (bindings[i].bindType == 2 && bindings[i].buttons == buttons) {
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
        unassign(getByButtons(buttons));
    }

    void bindKeyboardExecFile(const std::string& trigger, const std::string& filename) {
        unbindKeyboard(trigger);
        auto hk = getUnassigned();
        if (hk == nullptr) { return; } // err: max hotkeys bound
        hk->buttons = keyboardTriggerToButtons(trigger);
        hk->bindType = 2; // keyboard
        hk->execType = 1; // AS file
        hk->arg = filename;
    }

    void bindKeyboardExecFileNXTas(const std::string& trigger, const std::string& filename) {
        unbindKeyboard(trigger);
        auto hk = getUnassigned();
        if (hk == nullptr) { return; } // err: max hotkeys bound
        hk->buttons = keyboardTriggerToButtons(trigger);
        hk->bindType = 2; // keyboard
        hk->execType = 2; // nxtas file
        hk->arg = filename;
    }

    void bindKeyboardEval(const std::string& trigger, const std::string& scriptText) {
        unbindKeyboard(trigger);
        auto hk = getUnassigned();
        if (hk == nullptr) { return; } // err: max hotkeys bound
        hk->buttons = keyboardTriggerToButtons(trigger);
        hk->bindType = 2; // keyboard
        hk->execType = 3; // AS eval
        hk->arg = scriptText;
    }

    void unbindKeyboard(const std::string& trigger) {
        unassign(getByKeyboardTrigger(trigger));
    }

    u64 keyboardTriggerToButtons(const std::string& trigger) {
        // nn::hid::KeyboardKey fits in 1B so we can just pack up to 8 in the u64 buttons field.
        // this conversion from string is somewhat sloppy/expensive, but it's only run when registering/etc binds, not every frame.
        u64 outRaw = 0;
        u8* out = (u8*)(&outRaw);

        const char* tp = trigger.c_str(); // trigger pointer for unrolled loop
        const char* qs = ""; // query string for unrolled loop
        size_t ql = 0; // query string length
        nn::hid::KeyboardKey rv; // result value upon match
        #define KB_CMP_IMPL ql = strlen(qs); if (strncmp(tp, qs, ql) == 0 && (tp[ql] == ' ' || tp[ql] == '\0')) { goto KB_MATCH_IMPL; }

        while(true) {
            qs = "A"; rv = nn::hid::KeyboardKey::A; KB_CMP_IMPL
            qs = "B"; rv = nn::hid::KeyboardKey::B; KB_CMP_IMPL
            qs = "C"; rv = nn::hid::KeyboardKey::C; KB_CMP_IMPL
            qs = "D"; rv = nn::hid::KeyboardKey::D; KB_CMP_IMPL
            qs = "E"; rv = nn::hid::KeyboardKey::E; KB_CMP_IMPL
            qs = "F"; rv = nn::hid::KeyboardKey::F; KB_CMP_IMPL
            qs = "G"; rv = nn::hid::KeyboardKey::G; KB_CMP_IMPL
            qs = "H"; rv = nn::hid::KeyboardKey::H; KB_CMP_IMPL
            qs = "I"; rv = nn::hid::KeyboardKey::I; KB_CMP_IMPL
            qs = "J"; rv = nn::hid::KeyboardKey::J; KB_CMP_IMPL
            qs = "K"; rv = nn::hid::KeyboardKey::K; KB_CMP_IMPL
            qs = "L"; rv = nn::hid::KeyboardKey::L; KB_CMP_IMPL
            qs = "M"; rv = nn::hid::KeyboardKey::M; KB_CMP_IMPL
            qs = "N"; rv = nn::hid::KeyboardKey::N; KB_CMP_IMPL
            qs = "O"; rv = nn::hid::KeyboardKey::O; KB_CMP_IMPL
            qs = "P"; rv = nn::hid::KeyboardKey::P; KB_CMP_IMPL
            qs = "Q"; rv = nn::hid::KeyboardKey::Q; KB_CMP_IMPL
            qs = "R"; rv = nn::hid::KeyboardKey::R; KB_CMP_IMPL
            qs = "S"; rv = nn::hid::KeyboardKey::S; KB_CMP_IMPL
            qs = "T"; rv = nn::hid::KeyboardKey::T; KB_CMP_IMPL
            qs = "U"; rv = nn::hid::KeyboardKey::U; KB_CMP_IMPL
            qs = "V"; rv = nn::hid::KeyboardKey::V; KB_CMP_IMPL
            qs = "W"; rv = nn::hid::KeyboardKey::W; KB_CMP_IMPL
            qs = "X"; rv = nn::hid::KeyboardKey::X; KB_CMP_IMPL
            qs = "Y"; rv = nn::hid::KeyboardKey::Y; KB_CMP_IMPL
            qs = "Z"; rv = nn::hid::KeyboardKey::Z; KB_CMP_IMPL
            qs = "D1"; rv = nn::hid::KeyboardKey::D1; KB_CMP_IMPL
            qs = "D2"; rv = nn::hid::KeyboardKey::D2; KB_CMP_IMPL
            qs = "D3"; rv = nn::hid::KeyboardKey::D3; KB_CMP_IMPL
            qs = "D4"; rv = nn::hid::KeyboardKey::D4; KB_CMP_IMPL
            qs = "D5"; rv = nn::hid::KeyboardKey::D5; KB_CMP_IMPL
            qs = "D6"; rv = nn::hid::KeyboardKey::D6; KB_CMP_IMPL
            qs = "D7"; rv = nn::hid::KeyboardKey::D7; KB_CMP_IMPL
            qs = "D8"; rv = nn::hid::KeyboardKey::D8; KB_CMP_IMPL
            qs = "D9"; rv = nn::hid::KeyboardKey::D9; KB_CMP_IMPL
            qs = "D0"; rv = nn::hid::KeyboardKey::D0; KB_CMP_IMPL
            qs = "Return"; rv = nn::hid::KeyboardKey::Return; KB_CMP_IMPL
            qs = "Escape"; rv = nn::hid::KeyboardKey::Escape; KB_CMP_IMPL
            qs = "Backspace"; rv = nn::hid::KeyboardKey::Backspace; KB_CMP_IMPL
            qs = "Tab"; rv = nn::hid::KeyboardKey::Tab; KB_CMP_IMPL
            qs = "Space"; rv = nn::hid::KeyboardKey::Space; KB_CMP_IMPL
            qs = "Minus"; rv = nn::hid::KeyboardKey::Minus; KB_CMP_IMPL
            qs = "Plus"; rv = nn::hid::KeyboardKey::Plus; KB_CMP_IMPL
            qs = "OpenBracket"; rv = nn::hid::KeyboardKey::OpenBracket; KB_CMP_IMPL
            qs = "CloseBracket"; rv = nn::hid::KeyboardKey::CloseBracket; KB_CMP_IMPL
            qs = "Pipe"; rv = nn::hid::KeyboardKey::Pipe; KB_CMP_IMPL
            qs = "Tilde"; rv = nn::hid::KeyboardKey::Tilde; KB_CMP_IMPL
            qs = "Semicolon"; rv = nn::hid::KeyboardKey::Semicolon; KB_CMP_IMPL
            qs = "Quote"; rv = nn::hid::KeyboardKey::Quote; KB_CMP_IMPL
            qs = "Backquote"; rv = nn::hid::KeyboardKey::Backquote; KB_CMP_IMPL
            qs = "Comma"; rv = nn::hid::KeyboardKey::Comma; KB_CMP_IMPL
            qs = "Period"; rv = nn::hid::KeyboardKey::Period; KB_CMP_IMPL
            qs = "Slash"; rv = nn::hid::KeyboardKey::Slash; KB_CMP_IMPL
            qs = "CapsLock"; rv = nn::hid::KeyboardKey::CapsLock; KB_CMP_IMPL
            qs = "F1"; rv = nn::hid::KeyboardKey::F1; KB_CMP_IMPL
            qs = "F2"; rv = nn::hid::KeyboardKey::F2; KB_CMP_IMPL
            qs = "F3"; rv = nn::hid::KeyboardKey::F3; KB_CMP_IMPL
            qs = "F4"; rv = nn::hid::KeyboardKey::F4; KB_CMP_IMPL
            qs = "F5"; rv = nn::hid::KeyboardKey::F5; KB_CMP_IMPL
            qs = "F6"; rv = nn::hid::KeyboardKey::F6; KB_CMP_IMPL
            qs = "F7"; rv = nn::hid::KeyboardKey::F7; KB_CMP_IMPL
            qs = "F8"; rv = nn::hid::KeyboardKey::F8; KB_CMP_IMPL
            qs = "F9"; rv = nn::hid::KeyboardKey::F9; KB_CMP_IMPL
            qs = "F10"; rv = nn::hid::KeyboardKey::F10; KB_CMP_IMPL
            qs = "F11"; rv = nn::hid::KeyboardKey::F11; KB_CMP_IMPL
            qs = "F12"; rv = nn::hid::KeyboardKey::F12; KB_CMP_IMPL
            qs = "PrintScreen"; rv = nn::hid::KeyboardKey::PrintScreen; KB_CMP_IMPL
            qs = "ScrollLock"; rv = nn::hid::KeyboardKey::ScrollLock; KB_CMP_IMPL
            qs = "Pause"; rv = nn::hid::KeyboardKey::Pause; KB_CMP_IMPL
            qs = "Insert"; rv = nn::hid::KeyboardKey::Insert; KB_CMP_IMPL
            qs = "Home"; rv = nn::hid::KeyboardKey::Home; KB_CMP_IMPL
            qs = "PageUp"; rv = nn::hid::KeyboardKey::PageUp; KB_CMP_IMPL
            qs = "Delete"; rv = nn::hid::KeyboardKey::Delete; KB_CMP_IMPL
            qs = "End"; rv = nn::hid::KeyboardKey::End; KB_CMP_IMPL
            qs = "PageDown"; rv = nn::hid::KeyboardKey::PageDown; KB_CMP_IMPL
            qs = "RightArrow"; rv = nn::hid::KeyboardKey::RightArrow; KB_CMP_IMPL
            qs = "LeftArrow"; rv = nn::hid::KeyboardKey::LeftArrow; KB_CMP_IMPL
            qs = "DownArrow"; rv = nn::hid::KeyboardKey::DownArrow; KB_CMP_IMPL
            qs = "UpArrow"; rv = nn::hid::KeyboardKey::UpArrow; KB_CMP_IMPL
            qs = "NumLock"; rv = nn::hid::KeyboardKey::NumLock; KB_CMP_IMPL
            qs = "NumPadDivide"; rv = nn::hid::KeyboardKey::NumPadDivide; KB_CMP_IMPL
            qs = "NumPadMultiply"; rv = nn::hid::KeyboardKey::NumPadMultiply; KB_CMP_IMPL
            qs = "NumPadSubtract"; rv = nn::hid::KeyboardKey::NumPadSubtract; KB_CMP_IMPL
            qs = "NumPadAdd"; rv = nn::hid::KeyboardKey::NumPadAdd; KB_CMP_IMPL
            qs = "NumPadEnter"; rv = nn::hid::KeyboardKey::NumPadEnter; KB_CMP_IMPL
            qs = "NumPad1"; rv = nn::hid::KeyboardKey::NumPad1; KB_CMP_IMPL
            qs = "NumPad2"; rv = nn::hid::KeyboardKey::NumPad2; KB_CMP_IMPL
            qs = "NumPad3"; rv = nn::hid::KeyboardKey::NumPad3; KB_CMP_IMPL
            qs = "NumPad4"; rv = nn::hid::KeyboardKey::NumPad4; KB_CMP_IMPL
            qs = "NumPad5"; rv = nn::hid::KeyboardKey::NumPad5; KB_CMP_IMPL
            qs = "NumPad6"; rv = nn::hid::KeyboardKey::NumPad6; KB_CMP_IMPL
            qs = "NumPad7"; rv = nn::hid::KeyboardKey::NumPad7; KB_CMP_IMPL
            qs = "NumPad8"; rv = nn::hid::KeyboardKey::NumPad8; KB_CMP_IMPL
            qs = "NumPad9"; rv = nn::hid::KeyboardKey::NumPad9; KB_CMP_IMPL
            qs = "NumPad0"; rv = nn::hid::KeyboardKey::NumPad0; KB_CMP_IMPL
            qs = "NumPadDot"; rv = nn::hid::KeyboardKey::NumPadDot; KB_CMP_IMPL
            qs = "Backslash"; rv = nn::hid::KeyboardKey::Backslash; KB_CMP_IMPL
            qs = "Application"; rv = nn::hid::KeyboardKey::Application; KB_CMP_IMPL
            qs = "Power"; rv = nn::hid::KeyboardKey::Power; KB_CMP_IMPL
            qs = "NumPadEquals"; rv = nn::hid::KeyboardKey::NumPadEquals; KB_CMP_IMPL
            qs = "F13"; rv = nn::hid::KeyboardKey::F13; KB_CMP_IMPL
            qs = "F14"; rv = nn::hid::KeyboardKey::F14; KB_CMP_IMPL
            qs = "F15"; rv = nn::hid::KeyboardKey::F15; KB_CMP_IMPL
            qs = "F16"; rv = nn::hid::KeyboardKey::F16; KB_CMP_IMPL
            qs = "F17"; rv = nn::hid::KeyboardKey::F17; KB_CMP_IMPL
            qs = "F18"; rv = nn::hid::KeyboardKey::F18; KB_CMP_IMPL
            qs = "F19"; rv = nn::hid::KeyboardKey::F19; KB_CMP_IMPL
            qs = "F20"; rv = nn::hid::KeyboardKey::F20; KB_CMP_IMPL
            qs = "F21"; rv = nn::hid::KeyboardKey::F21; KB_CMP_IMPL
            qs = "F22"; rv = nn::hid::KeyboardKey::F22; KB_CMP_IMPL
            qs = "F23"; rv = nn::hid::KeyboardKey::F23; KB_CMP_IMPL
            qs = "F24"; rv = nn::hid::KeyboardKey::F24; KB_CMP_IMPL
            qs = "NumPadComma"; rv = nn::hid::KeyboardKey::NumPadComma; KB_CMP_IMPL
            qs = "Ro"; rv = nn::hid::KeyboardKey::Ro; KB_CMP_IMPL
            qs = "KatakanaHiragana"; rv = nn::hid::KeyboardKey::KatakanaHiragana; KB_CMP_IMPL
            qs = "Yen"; rv = nn::hid::KeyboardKey::Yen; KB_CMP_IMPL
            qs = "Henkan"; rv = nn::hid::KeyboardKey::Henkan; KB_CMP_IMPL
            qs = "Muhenkan"; rv = nn::hid::KeyboardKey::Muhenkan; KB_CMP_IMPL
            qs = "NumPadCommaPc98"; rv = nn::hid::KeyboardKey::NumPadCommaPc98; KB_CMP_IMPL
            qs = "HangulEnglish"; rv = nn::hid::KeyboardKey::HangulEnglish; KB_CMP_IMPL
            qs = "Hanja"; rv = nn::hid::KeyboardKey::Hanja; KB_CMP_IMPL
            qs = "Katakana"; rv = nn::hid::KeyboardKey::Katakana; KB_CMP_IMPL
            qs = "Hiragana"; rv = nn::hid::KeyboardKey::Hiragana; KB_CMP_IMPL
            qs = "ZenkakuHankaku"; rv = nn::hid::KeyboardKey::ZenkakuHankaku; KB_CMP_IMPL
            qs = "LeftControl"; rv = nn::hid::KeyboardKey::LeftControl; KB_CMP_IMPL
            qs = "LeftShift"; rv = nn::hid::KeyboardKey::LeftShift; KB_CMP_IMPL
            qs = "LeftAlt"; rv = nn::hid::KeyboardKey::LeftAlt; KB_CMP_IMPL
            qs = "LeftGui"; rv = nn::hid::KeyboardKey::LeftGui; KB_CMP_IMPL
            qs = "RightControl"; rv = nn::hid::KeyboardKey::RightControl; KB_CMP_IMPL
            qs = "RightShift"; rv = nn::hid::KeyboardKey::RightShift; KB_CMP_IMPL
            qs = "RightAlt"; rv = nn::hid::KeyboardKey::RightAlt; KB_CMP_IMPL
            qs = "RightGui"; rv = nn::hid::KeyboardKey::RightGui; KB_CMP_IMPL

            break; // empty/invalid trigger

            KB_MATCH_IMPL:
            *out = static_cast<u8>(rv); // store this bit index
            out++; // FIXME assert 8 token max
            tp += ql;
            if (*tp == '\0') { break; } // end of trigger reached
            else { tp++; } // skip space
        }

        #undef KB_CMP_IMPL
        return outRaw;
    }

} // ns

