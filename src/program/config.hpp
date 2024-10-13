#pragma once
#include <cstring>
#include "util/romfs.hpp"
#include "lib/json.hpp"
using json = nlohmann::json;

namespace lotuskit::config {
    json jsonConfig;

    void LoadJson() {
        constexpr const char* JSON_PATH = "content:/totk_lotuskit/config.json";
        constexpr const char* JSON_PRESET_DISABLE = R"({"disable": true})";
        constexpr u32 JSON_MAX_FILESIZE = 0x2000;
        char buf[JSON_MAX_FILESIZE];

        if (!lotuskit::util::romfs::FileExists(JSON_PATH)) {
            nn::util::SNPrintf(buf, sizeof(buf), "[ERROR] config file %s missing", JSON_PATH);
            svcOutputDebugString(buf, strlen(buf));
            jsonConfig = json::parse(JSON_PRESET_DISABLE);
        } else {
            lotuskit::util::romfs::ReadTextFile(buf, JSON_PATH, JSON_MAX_FILESIZE, JSON_PRESET_DISABLE);
            jsonConfig = json::parse(buf, nullptr, false, true); // ..., ignore_comments=true
        }

        nn::util::SNPrintf(buf, sizeof(buf), "Loaded %s as: %s", JSON_PATH, jsonConfig.dump(4).c_str()); // visual bug: will be truncated near JSON_MAX_FILESIZE
        svcOutputDebugString(buf, strlen(buf));
    }
}
