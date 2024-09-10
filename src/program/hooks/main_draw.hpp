#pragma once
#include <utility>
#include "helpers/InputHelper.hpp"
#include "lib.hpp"
#include "logger/Logger.hpp"
#include "nn/nn.h"
#include "nn/hid.h"
#include "nn/util.h"

// agl include
#include "common/aglDrawContext.h"
#include "lyr/aglLayer.h"
#include "lyr/aglRenderInfo.h"

// agl sym
#include "sym/lyr/aglLayer.h"

// sead include
#include "gfx/seadColor.h"
#include "gfx/seadDrawContext.h"
#include "gfx/seadTextWriter.h"
#include "math/seadVector.hpp"

// sead sym
#include "sym/gfx/seadTextWriter.h"

extern ModCommand_ActorWatcher g_ModCommand_ActorWatcher[4]; // XXX trash glue
extern ModCommand_Hexdump g_ModCommand_Hexdump[4]; // XXX trash glue

class TextWriterExt: public sead::TextWriter {
    public:
    void getCursorFromTopLeftImpl(sead::Vector2f* pos) const {
        // always 720p sooooo just make it work
        pos->x = this->mCursor.x + 640.0;
        pos->y = 360.0 - this->mCursor.y;
    }
    void pprintf(sead::Vector2f &pos, const char* fmt, auto&&... args) {
        // pretty print: text shadow drawn first
        this->mColor.r = 0.0; this->mColor.g = 0.0; this->mColor.b = 0.0; this->mColor.a = 1.0; // black
        this->setCursorFromTopLeft(pos);
        this->printf(fmt, std::forward<decltype(args)>(args)...); // javascript lookin ass bullshit

        // main draw
        this->mColor.r = 0.85; this->mColor.g = 0.85; this->mColor.b = 0.85; this->mColor.a = 1.0; // white ish
        pos.x -= 1.0;
        pos.y -= 1.0;
        this->setCursorFromTopLeft(pos);
        this->printf(fmt, std::forward<decltype(args)>(args)...);

        // remember position for future calls
        this->getCursorFromTopLeftImpl(&pos);
        pos.x += 1.0;
        pos.y += 1.0;
    }
};


HOOK_DEFINE_TRAMPOLINE(DebugDrawImpl) {
    static const ptrdiff_t s_offset = sym::agl::lyr::Layer::drawDebugInfo_;
    static void Callback(agl::lyr::Layer* layer, const agl::lyr::RenderInfo& info) {
        // draw onto the given layer, always Tool 2D -- we would be given many layers if they weren't ignored above

        // XXX do we really need to do this every time? weird its a static. maybe we can hold onto one instead
        auto* sead_draw_ctx = dynamic_cast<sead::DrawContext*>(info.draw_ctx);
        sead::TextWriter::setupGraphics(sead_draw_ctx);
        sead::TextWriter _writer(sead_draw_ctx, info.viewport);
        TextWriterExt* writer = (TextWriterExt*)(&_writer);

        sead::Vector2f text_pos; // screen is always represented as 1280x720, upscaled for 1080p
        // scale: 0.665 @ 1080p = 1.0 at 720p. The re-up-scaling means reducing size here looks bad fast, 0.8 is legible
        writer->mScale.x = 0.8;
        writer->mScale.y = 0.8;
        text_pos.x = 2.0;
        text_pos.y = 2.0;

        // no Player? either bootup or something very bad, unconditionally show feedback + version confirmation (if it'll even render) while we wait
        // TODO source+display high level configs (socket, svclog, etc)
        if (g_ModCommand_ActorWatcher[0].actor == nullptr) {
            writer->pprintf(text_pos, "[totk-lotuskit:%d] awaiting Player, main_offset=%p\n", TOTK_VERSION, exl::util::GetMainModuleInfo().m_Total.m_Start);
        }

        // TODO extract ActorWatcher::draw(sead::Vector2f pos, TextWriterExt* writer)
        if (true) {
            for (u8 i=0; i < std::size(g_ModCommand_ActorWatcher); i++) {
                auto &cmd = g_ModCommand_ActorWatcher[i];
                if (!cmd.is_publishing_selection || cmd.actor == nullptr) { continue; }
                const auto actor = cmd.actor;
                const auto rot = actor->mRotationMatrix;
                hknpMotion* motion = cmd.tracked_motion;

                if (motion != nullptr) {
                    const auto pos64 = motion->m_centerOfMass;
                    writer->pprintf(text_pos, "%s\npos64: %f, %f, %f \nrot_physics: %f, %f, %f, %f \nphysics_ang_vel: %f, %f, %f \nphysics_vel: %f, %f, %f \ninertia: %f, %f, %f, %f \npos32: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f] \n\n",
                        actor->mIActor.name,
                        // hknpMotion
                        pos64.X, pos64.Y, pos64.Z,
                        motion->m_orientation.A, motion->m_orientation.B, motion->m_orientation.C, motion->m_orientation.D,
                        motion->m_angularVelocityLocalAndSpeedLimit.X, motion->m_angularVelocityLocalAndSpeedLimit.Y, motion->m_angularVelocityLocalAndSpeedLimit.Z,
                        motion->m_linearVelocityAndSpeedLimit.X, motion->m_linearVelocityAndSpeedLimit.Y, motion->m_linearVelocityAndSpeedLimit.Z,
                        motion->m_inverseInertia[0], motion->m_inverseInertia[1], motion->m_inverseInertia[2], motion->m_inverseInertia[3],
                        // Actor
                        actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                        rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                    );

                } else {
                    // Actor only
                    writer->pprintf(text_pos, "%s\npos32: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f]\n\n",
                        actor->mIActor.name,
                        actor->mPosition.X, actor->mPosition.Y, actor->mPosition.Z,
                        rot.m11, rot.m12, rot.m13, rot.m21, rot.m22, rot.m23, rot.m31, rot.m32, rot.m33
                    );
                }
            }
        }

        // TODO extract ModCommand_Hexdump::draw
        if (true) {
            for (u8 i=0; i < std::size(g_ModCommand_Hexdump); i++) {
                auto &cmd = g_ModCommand_Hexdump[i];
                if (!cmd.is_draw || cmd.dump_src == nullptr) { continue; }

                const char* live = cmd.calc_age == 0 ? "[LIVE]" : ""; // visible when buf has been updated this frame, for slow/intermittent/triggered/discontinued/etc dumps
                writer->pprintf(text_pos, "%s[%d](dump_len=%d, age=%d) %s\n", cmd.label, i, cmd.dump_len, cmd.calc_age, live);

                u8* backing_src = (u8*)cmd.dump_src;
                u8* src = cmd.buf;
                u32 row = 0;
                do { if (row * 0x10 >= cmd.draw_len) { break; }
                    writer->pprintf(text_pos,
                        "%p | %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                        backing_src, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9], src[10], src[11], src[12], src[13], src[14], src[15]
                    );

                    row++;
                    backing_src += 0x10;
                    src += 0x10;
                } while (true);
            }
        }

        writer->mScale.x = 0.5; // but we dont need legible here
        writer->mScale.y = 0.5;
        text_pos.x = 1280.0 - 75.0;
        text_pos.y = 2.0;
        // TODO extract InputDisplay::draw(sead::Vector2f pos, TextWriterExt* writer)
        if (true) {
            const char* a       = InputHelper::isButtonHold(nn::hid::NpadButton::A)     ? "A" : " ";
            const char* b       = InputHelper::isButtonHold(nn::hid::NpadButton::B)     ? "B" : " ";
            const char* x       = InputHelper::isButtonHold(nn::hid::NpadButton::X)     ? "X" : " ";
            const char* y       = InputHelper::isButtonHold(nn::hid::NpadButton::Y)     ? "Y" : " ";
            const char* l       = InputHelper::isButtonHold(nn::hid::NpadButton::L)     ? "L" : " ";
            const char* r       = InputHelper::isButtonHold(nn::hid::NpadButton::R)     ? "R" : " ";
            const char* zl      = InputHelper::isButtonHold(nn::hid::NpadButton::ZL)    ? "ZL": "  ";
            const char* zr      = InputHelper::isButtonHold(nn::hid::NpadButton::ZR)    ? "ZR": "  ";
            const char* plus    = InputHelper::isButtonHold(nn::hid::NpadButton::Plus)  ? "+" : " ";
            const char* minus   = InputHelper::isButtonHold(nn::hid::NpadButton::Minus) ? "-" : " ";
            const char* d_left  = InputHelper::isButtonHold(nn::hid::NpadButton::Left)  ? "<" : " ";
            const char* d_up    = InputHelper::isButtonHold(nn::hid::NpadButton::Up)    ? "^" : " ";
            const char* d_right = InputHelper::isButtonHold(nn::hid::NpadButton::Right) ? ">" : " ";
            const char* d_down  = InputHelper::isButtonHold(nn::hid::NpadButton::Down)  ? "v" : " ";
            // TODO analog display
            const char* stick_l = InputHelper::isButtonHold(nn::hid::NpadButton::StickL)      ? "L" : " ";
            const char* l_left  = InputHelper::isButtonHold(nn::hid::NpadButton::StickLLeft)  ? "<" : " ";
            const char* l_up    = InputHelper::isButtonHold(nn::hid::NpadButton::StickLUp)    ? "^" : " ";
            const char* l_right = InputHelper::isButtonHold(nn::hid::NpadButton::StickLRight) ? ">" : " ";
            const char* l_down  = InputHelper::isButtonHold(nn::hid::NpadButton::StickLDown)  ? "v" : " ";
            const char* stick_r = InputHelper::isButtonHold(nn::hid::NpadButton::StickR)      ? "R" : " ";
            const char* r_left  = InputHelper::isButtonHold(nn::hid::NpadButton::StickRLeft)  ? "<" : " ";
            const char* r_up    = InputHelper::isButtonHold(nn::hid::NpadButton::StickRUp)    ? "^" : " ";
            const char* r_right = InputHelper::isButtonHold(nn::hid::NpadButton::StickRRight) ? ">" : " ";
            const char* r_down  = InputHelper::isButtonHold(nn::hid::NpadButton::StickRDown)  ? "v" : " ";
            //const char bq* = InputHelper::isKeyHold(nn::hid::KeyboardKey::Backquote) ? "`" : " ";
            // TODO does game respond to any of these?
            //LeftSL = 24,
            //LeftSR = 25,
            //RightSL = 26,
            //RightSR = 27,
            //Palma = 28,
            //Verification = 29,
            //HandheldLeftB = 30,  // (Left B button on NES controllers in Handheld mode)
            //LeftC = 31,          // [12.0.0+] (Left C button in N64 controller)
            //UpC = 32,            // [12.0.0+] (Up C button in N64 controller)
            //RightC = 33,         // [12.0.0+] (Right C button in N64 controller)
            //DownC = 34,          // [12.0.0+] (Down C button in N64 controller)

            // All lit up:
            //ZL L ^- +^  X R ZR
            //    <L>^<R>Y A
            //     v<v>v  B
            const char* inputs_fmt = "%s %s %s%s %s%s  %s %s %s\n    %s%s%s%s%s%s%s%s %s\n     %s%s%s%s%s  %s\n";
            writer->pprintf(
                text_pos, inputs_fmt,
                zl, l, l_up, minus, plus, r_up, x, r, zr,
                l_left, stick_l, l_right, d_up, r_left, stick_r, r_right, y, a,
                l_down, d_left, d_down, d_right, r_down, b
            );
        }
    }
};

