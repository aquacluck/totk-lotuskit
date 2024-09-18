#pragma once

namespace sym::sead::TextWriter {
    // _ZN4sead10TextWriter20setCursorFromTopLeftERKNS_7Vector2IfEE
    // sead::TextWriter::setCursorFromTopLeft(sead::Vector2<float> const&)
    static constexpr ptrdiff_t setCursorFromTopLeft = 0x00000000;

    // _ZN4sead10TextWriter6printfEPKcz
    // sead::TextWriter::printf(char const*, ...)
    static constexpr ptrdiff_t printf = 0x01899004;

    // _ZN4sead10TextWriterD1Ev
    // sead::TextWriter::{dtor}()
    static constexpr ptrdiff_t dtor = 0x00000000;

    // _ZN4sead10TextWriter13setupGraphicsEPNS_11DrawContextE
    // sead::TextWriter::setupGraphics(sead::DrawContext*)
    static constexpr ptrdiff_t setupGraphics = 0x01898d9c;

    // _ZN4sead10TextWriterC1EPNS_11DrawContextEPKNS_8ViewportE
    // sead::TextWriter::{ctor}(sead::DrawContext*, sead::Viewport const*)
    static constexpr ptrdiff_t ctor = 0x010c85b8;
}
