#pragma once
#include "lib/json.hpp"
using json = nlohmann::json;

namespace lotuskit {
    class Config {
        public:
        static json jsonConfig;
        static void LoadJson();
    };
} // ns
