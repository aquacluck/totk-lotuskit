#pragma once

namespace sym::sead::TextWriter {
    // _ZN4sead10TextWriter13setupGraphicsEPNS_11DrawContextE
    // sead::TextWriter::setupGraphics(sead::DrawContext*)
    static constexpr ptrdiff_t setupGraphics = 0x0189917c;

    // _ZN4sead10TextWriter20setCursorFromTopLeftERKNS_7Vector2IfEE
    // sead::TextWriter::setCursorFromTopLeft(sead::Vector2<float> const&)
    static constexpr ptrdiff_t setCursorFromTopLeft = 0x010bfdb4;

    // _ZN4sead10TextWriter6printfEPKcz
    // sead::TextWriter::printf(char const*, ...)
    static constexpr ptrdiff_t printf = 0x018993e4;

    // _ZN4sead10TextWriterC1EPNS_11DrawContextEPKNS_8ViewportE
    // sead::TextWriter::{ctor}(sead::DrawContext*, sead::Viewport const*)
    static constexpr ptrdiff_t ctor = 0x010bfdf8;

    // _ZN4sead10TextWriterD1Ev
    // sead::TextWriter::{dtor}()
    static constexpr ptrdiff_t dtor = 0x010bfd60;
}
