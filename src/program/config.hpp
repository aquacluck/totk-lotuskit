#pragma once
#include <cstring>
#include "util/romfs.hpp"
#include "json.hpp"
using json = nlohmann::json;

namespace lotuskit::config {
    constexpr const char* JSON_PATH = "content:/totk_lotuskit/config.json";
    constexpr const char* JSON_PRESET_DISABLE = R"({"disable": true})";
    constexpr u32 JSON_MAX_FILESIZE = 0x500; // dont go crazy with it, just use more files when it makes sense
    json json_config;

    void LoadJson() {
        char buf[JSON_MAX_FILESIZE];

        if (!lotuskit::util::romfs::FileExists(JSON_PATH)) {
            nn::util::SNPrintf(buf, sizeof(buf), "[ERROR] config file %s missing", JSON_PATH);
            svcOutputDebugString(buf, strlen(buf));
            json_config = json::parse(JSON_PRESET_DISABLE);
        } else {
            lotuskit::util::romfs::ReadFile(buf, JSON_PATH, JSON_MAX_FILESIZE, JSON_PRESET_DISABLE);
            json_config = json::parse(buf, nullptr, false, true); // ..., ignore_comments=true
        }

        nn::util::SNPrintf(buf, sizeof(buf), "Loaded %s as: %s", JSON_PATH, json_config.dump(4).c_str()); // visual bug: will be truncated near JSON_MAX_FILESIZE
        svcOutputDebugString(buf, strlen(buf));
    }
}
