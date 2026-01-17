#pragma once

namespace ai {

class ControllerBase {
public:
    virtual ~ControllerBase();

protected:
    char _00[0x1f0]; // just a skeleton for now
};
static_assert(sizeof(ControllerBase) == 0x1f8);

} // namespace ai
