#pragma once

#include "types.h"

namespace sead {

class DirectCamera;
class Projection;
class Viewport;

} // namespace sead

namespace agl {

class DrawContext;

namespace lyr {

class Layer;
class RenderBuffer;

struct RenderInfo {
    int _00;
    int _04;
    u8 _08;
    char _09[0xf];
    RenderBuffer* render_buffer0;
    RenderBuffer* render_buffer1;
    Layer* layer;
    int _30;
    int _34;
    sead::DirectCamera* camera;
    sead::Projection* projection;
    sead::Viewport* viewport;
    u8 _50;
    char _51[7];
    DrawContext* draw_ctx;
};

} // namespace lyr

} // namespace agl
