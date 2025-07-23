#pragma once
#include <cstdarg>
#include "exlaunch.hpp"
#include "util/alloc.hpp"
using json = lotuskit::json;
using String = lotuskit::String;

namespace lotuskit {
    class Logger {
        public:
        inline static const json::json_pointer EXECNS = json::json_pointer("/Logger");

        static void logJson(json obj, const String& ns=EXECNS, bool doBlocking=false, bool doDebugLog=true); // must be an object! not an array
        static void logText(const String& msg, const String& ns=EXECNS, bool doBlocking=false, bool doDebugLog=true); // sugar: wraps msg into json obj
        static void dumpTextFileIntoNS(const String& filename, const String& ns);
        static void dumpDirectoryIndexIntoNS(const String& path, const String& ns);
    };

} // ns
