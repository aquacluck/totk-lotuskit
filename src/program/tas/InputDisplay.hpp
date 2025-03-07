#pragma once
#include "exlaunch.hpp"
#include "structs/nnSixAxis.hpp"

namespace lotuskit::tas::InputDisplay {
    void draw();
    void drawInputDisplay(nn::hid::NpadBaseState* input);
    void drawGyro(nn::hid::SixAxisSensorState* gyro);

    inline bool doTextWriterInputDisplay = true;
    inline void doTextWriterInputDisplay_set(bool v) { doTextWriterInputDisplay = v; }
    inline bool doTextWriterGyro = false;
    inline void doTextWriterGyro_set(bool v) { doTextWriterGyro = v; }
}
