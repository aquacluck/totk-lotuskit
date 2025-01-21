#include "TextWriter.hpp"
#include "tas/InputDisplay.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"

namespace lotuskit::tas {
    void InputDisplay::draw() {
        u64 buttons = 0;
        s32 LX, LY, RX, RY;

        if (lotuskit::tas::Playback::isPlaybackActive && !lotuskit::tas::Playback::isSleepInput) {
            auto& input = lotuskit::tas::Playback::currentInput;
            buttons = *(u64*)&(input.buttons);
            LX = input.LStick.mX;
            LY = input.LStick.mY;
            RX = input.RStick.mX;
            RY = input.RStick.mY;
        } else {
            // contains latest polled input even when not recording
            auto& input = lotuskit::tas::Record::currentInput;
            buttons = *(u64*)&(input.buttons);
            LX = input.LStick.mX;
            LY = input.LStick.mY;
            RX = input.RStick.mX;
            RY = input.RStick.mY;
        }

        const char* a       = buttons & (1 << (u32)nn::hid::NpadButton::A)     ? "A" : " ";
        const char* b       = buttons & (1 << (u32)nn::hid::NpadButton::B)     ? "B" : " ";
        const char* x       = buttons & (1 << (u32)nn::hid::NpadButton::X)     ? "X" : " ";
        const char* y       = buttons & (1 << (u32)nn::hid::NpadButton::Y)     ? "Y" : " ";
        const char* l       = buttons & (1 << (u32)nn::hid::NpadButton::L)     ? "L" : " ";
        const char* r       = buttons & (1 << (u32)nn::hid::NpadButton::R)     ? "R" : " ";
        const char* zl      = buttons & (1 << (u32)nn::hid::NpadButton::ZL)    ? "ZL": "  ";
        const char* zr      = buttons & (1 << (u32)nn::hid::NpadButton::ZR)    ? "ZR": "  ";
        const char* plus    = buttons & (1 << (u32)nn::hid::NpadButton::Plus)  ? "+" : " ";
        const char* minus   = buttons & (1 << (u32)nn::hid::NpadButton::Minus) ? "-" : " ";
        const char* d_left  = buttons & (1 << (u32)nn::hid::NpadButton::Left)  ? "<" : " ";
        const char* d_up    = buttons & (1 << (u32)nn::hid::NpadButton::Up)    ? "^" : " ";
        const char* d_right = buttons & (1 << (u32)nn::hid::NpadButton::Right) ? ">" : " ";
        const char* d_down  = buttons & (1 << (u32)nn::hid::NpadButton::Down)  ? "v" : " ";
        // TODO analog display
        const char* stick_l = buttons & (1 << (u32)nn::hid::NpadButton::StickL)      ? "L" : " ";
        const char* l_left  = buttons & (1 << (u32)nn::hid::NpadButton::StickLLeft)  ? "<" : " ";
        const char* l_up    = buttons & (1 << (u32)nn::hid::NpadButton::StickLUp)    ? "^" : " ";
        const char* l_right = buttons & (1 << (u32)nn::hid::NpadButton::StickLRight) ? ">" : " ";
        const char* l_down  = buttons & (1 << (u32)nn::hid::NpadButton::StickLDown)  ? "v" : " ";
        const char* stick_r = buttons & (1 << (u32)nn::hid::NpadButton::StickR)      ? "R" : " ";
        const char* r_left  = buttons & (1 << (u32)nn::hid::NpadButton::StickRLeft)  ? "<" : " ";
        const char* r_up    = buttons & (1 << (u32)nn::hid::NpadButton::StickRUp)    ? "^" : " ";
        const char* r_right = buttons & (1 << (u32)nn::hid::NpadButton::StickRRight) ? ">" : " ";
        const char* r_down  = buttons & (1 << (u32)nn::hid::NpadButton::StickRDown)  ? "v" : " ";

        // All lit up:
        //[tas::Playback] TOTK_XXX
        //     LX:-22222 RX:-22222
        //     LY:-22222 RY:-22222
        //     ZL L ^- +^   X R ZR
        //         <L>^<R> Y A
        //          v<v>v   B

        const char* inputs_fmt = "     LX:%6d RX:%6d\n     LY:%6d RY:%6d\n     %s %s %s%s %s%s   %s %s %s\n         %s%s%s%s%s%s%s %s %s\n          %s%s%s%s%s   %s\n";
        lotuskit::TextWriter::printf(1, inputs_fmt,
            LX, RX, LY, RY,
            zl, l, l_up, minus, plus, r_up, x, r, zr,
            l_left, stick_l, l_right, d_up, r_left, stick_r, r_right, y, a,
            l_down, d_left, d_down, d_right, r_down, b
        );
        return;
    }
}
