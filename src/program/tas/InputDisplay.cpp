#include <nn/hid.h>
#include "TextWriter.hpp"
#include "tas/InputDisplay.hpp"
#include "tas/Playback.hpp"
#include "tas/Record.hpp"

namespace lotuskit::tas::InputDisplay {
    void draw() {
        if (doTextWriterInputDisplay) {
            // copy human input: tas::Record contains latest polled input even when not recording
            nn::hid::NpadBaseState input = {0};
            std::memcpy((void*)&(input.mButtons), (void*)&(lotuskit::tas::Record::currentInput.buttons), 24);
            drawInputDisplay(&input);
        }

        if (doTextWriterGyro) {
            nn::hid::SixAxisSensorState gyro = {0};
            std::memcpy((void*)&(gyro), (void*)&(lotuskit::tas::Record::currentInput.gyro), sizeof(nn::hid::SixAxisSensorState));
            drawGyro(&gyro);
        }
    }

    void drawInputDisplay(nn::hid::NpadBaseState* input) {
        // alter/override/passthrough according to current options
        lotuskit::tas::Playback::applyCurrentInput(input);

        // display effective input values
        u64 buttons = *((u64*)&(input->mButtons));
        s32 LX = input->mAnalogStickL.mX;
        s32 LY = input->mAnalogStickL.mY;
        s32 RX = input->mAnalogStickR.mX;
        s32 RY = input->mAnalogStickR.mY;

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
    }

    void drawGyro(nn::hid::SixAxisSensorState* gyro) {
        // alter/override/passthrough according to current options
        lotuskit::tas::Playback::applyCurrentGyro(gyro);

        sead::Vector3f& acc = gyro->linearAcceleration;
        sead::Vector3f& angvel = gyro->angularVelocity;
        sead::Vector3f& angsum = gyro->angularVelocitySum;
        sead::Matrix33f& rot = gyro->rotation;
        lotuskit::TextWriter::appendCallback(1, [](lotuskit::TextWriterExt* writer, sead::Vector2f* textPos) {
            textPos->x -= 90.0; // pull left to make some more space
        });
        lotuskit::TextWriter::printf(1, "[gyro]\nlinacc: %f, %f, %f\nangvel: %f, %f, %f\nangsum: %f, %f, %f\n", acc.x, acc.y, acc.z, angvel.x, angvel.y, angvel.z, angsum.x, angsum.y, angsum.z);
        lotuskit::TextWriter::printf(1, "rot: %f, %f, %f\n     %f, %f, %f\n     %f, %f, %f\n\n", rot.a[0], rot.a[1], rot.a[2], rot.a[3], rot.a[4], rot.a[5], rot.a[6], rot.a[7], rot.a[8]);
    }
}
