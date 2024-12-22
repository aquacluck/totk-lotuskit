from build_types import *
begin_sym_file(__file__)

# namespace agl { static uintptr_t create_arg; }
EXKING("_ZN3aglL10create_argE", {
    TOTK_100: 0x00ea5118,
    TOTK_110: 0x00adb7cc,
    TOTK_111: 0x00b03db8,
    TOTK_112: 0x00a9c6b4,
    TOTK_120: 0x00ab0150,
    TOTK_121: 0x00a9123c,
}, SymbolType.INSTRUCTION)

# namespace agl::lyr::RenderDisplay::drawLayer_ { static uintptr_t ensure_font; }
# FIXME this does not ensure a font anymore, layer mask
EXKING("_ZN3agl3lyr13RenderDisplay10drawLayer_L11ensure_fontE", {
    TOTK_100: 0x00857f28,
    TOTK_110: 0x0083f5b8,
    TOTK_111: 0x00784e90,
    TOTK_112: 0x00808478,
    TOTK_120: 0x007d1fb0,
    TOTK_121: 0x00818340,
}, SymbolType.INSTRUCTION)

# namespace agl::lyr::Renderer { static uintptr_t draw; } XXX trampoline only
EXKING("_ZN3agl3lyr8RendererL4drawE", {
    TOTK_100: 0x00b983b4,
    TOTK_110: 0x00be8138,
    TOTK_121: 0x00bc81ac,
})

# namespace agl { static uintptr_t default_font; }
EXKING("_ZN3aglL12default_fontE", {
    TOTK_100: 0x0463c0f8,
    TOTK_110: 0x04719618,
    TOTK_111: 0x04721548,
    TOTK_112: 0x04713a28,
    TOTK_120: 0x04707e80,
    TOTK_121: 0x04716af8,
}, SymbolType.DATA)

# namespace agl { static uintptr_t init_debug_drawers; }
EXKING("_ZN3aglL18init_debug_drawersE", {
    TOTK_100: 0x00ea6834,
    TOTK_110: 0x00adcef4,
    TOTK_111: 0x00b054d8,
    TOTK_112: 0x00a9dddc,
    TOTK_120: 0x00ab1870,
    TOTK_121: 0x00a92964,
})

