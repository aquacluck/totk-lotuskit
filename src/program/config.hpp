#pragma once
#include "lib/json.hpp"
using json = nlohmann::json;

namespace lotuskit::config {
    extern json jsonConfig;
    void LoadJson();
}
