#pragma once
#include <cstdarg>
#include <string>
#include "lib.hpp"
#include "lib/json.hpp"
using json = nlohmann::json;

namespace lotuskit {
    class Logger {
        public:
        inline static const json::json_pointer EXECNS = json::json_pointer("/Logger");

        static void logJson(json obj, const std::string& ns=EXECNS, bool doBlocking=false, bool doDebugLog=true); // must be an object! not an array
        static void logText(const std::string& msg, const std::string& ns=EXECNS, bool doBlocking=false, bool doDebugLog=true); // sugar: wraps msg into json obj
    };

} // ns
