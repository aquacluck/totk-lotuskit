from build_types import *
begin_sym_file(__file__)

# namespace sead {class TextWriter { public: TextWriter(DrawContext* context, const Viewport* viewport); }; }
EXKING("_ZN4sead10TextWriterC1EPNS_11DrawContextEPKNS_8ViewportE", {
    TOTK_100: 0x0108ed1c,
    TOTK_110: 0x010bfdf8,
    TOTK_111: 0x010c85b8,
    TOTK_112: 0x010b4648,
    TOTK_120: 0x010ab4d8,
    TOTK_121: 0x010ad510,
})

# dtor
EXKING("_ZN4sead10TextWriterD1Ev", {
    TOTK_100: 0x029bc880,
    TOTK_110: 0x010bfd60,
    # TODO
    TOTK_121: 0x010ad478,
})

# namespace sead { class DrawContext; class TextWriter { public: static void setupGraphics(DrawContext* context); }; }
EXKING("_ZN4sead10TextWriter13setupGraphicsEPNS_11DrawContextE", {
    TOTK_100: 0x0184c778,
    TOTK_110: 0x0189917c,
    TOTK_111: 0x01898d9c,
    TOTK_112: 0x0188e060,
    TOTK_120: 0x0187a8d4,
    TOTK_121: 0x01888e6c,
})

# namespace sead { class Vector2f; class TextWriter { public: void setCursorFromTopLeft(const Vector2f& pos); }; }
EXKING("_ZN4sead10TextWriter20setCursorFromTopLeftERKNS_7Vector2IfEE", {
# FIXME _ZN4sead10TextWriter20setCursorFromTopLeftERKNS_8Vector2fE was expected? but my calls don't hit it
    TOTK_100: 0x0108ecd8,
    TOTK_110: 0x010bfdb4,
    # TODO
    TOTK_121: 0x010ad4cc,
})

# namespace sead { class TextWriter { public: void printf(const char* format, ...); }; }
EXKING("_ZN4sead10TextWriter6printfEPKcz", {
    TOTK_100: 0x0184c9e0,
    TOTK_110: 0x018993e4,
    TOTK_111: 0x01899004,
    TOTK_112: 0x0188e2c8,
    TOTK_120: 0x0187ab3c,
    TOTK_121: 0x018890d4,
})

#(&TextWriterBeginDraw) = reinterpret_cast<uintptr_t>(main + 0x010ad498);
#(&TextWriterEndDraw) = reinterpret_cast<uintptr_t>(main + 0x010ad478);
