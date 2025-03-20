#include <string>
#include "script/engine.hpp"
#include "server/JsonDispatch.hpp"
#include "util/fs.hpp"
#include "exlaunch.hpp"
#include "lib/json.hpp"
using json = nlohmann::json;


namespace lotuskit::server {
    void JsonDispatch::dispatchText(const char* jsonText) {
        json jsonPayload = json::parse(jsonText);
        if (jsonPayload.contains("execScriptString")) {
            auto moduleName = jsonPayload.contains("execScriptModule") ? (jsonPayload["execScriptModule"].template get<std::string>()) : "web.as";
            auto scriptText = jsonPayload["execScriptString"].template get<std::string>();
            lotuskit::script::engine::execTextInNewModule(moduleName.c_str(), "web.as", scriptText.c_str(), "void main()");

        } else if (jsonPayload.contains("persistFileString") && jsonPayload.contains("persistFileName")) {
            auto src = jsonPayload["persistFileString"].template get<std::string>();
            auto path = jsonPayload["persistFileName"].template get<std::string>();
            lotuskit::util::fs::writeTextFile(src.c_str(), path.c_str());

        } else if (jsonPayload.contains("persistFileBinary") && jsonPayload.contains("persistFileName")) {
            // FIXME this only needs to be good enough to overwrite subsdk9 (~2MB for now will be enough),
            //       then pipe a POST here once we have threaded httpd, then upload+relaunch will be fairly smooth.
            auto srcRaw = jsonPayload["persistFileBinary"].template get<std::string>();
            const void* src = "TODO"; // TODO decode json string to bin
            s64 srcLen = 4; // TODO set decoded size
            auto path = jsonPayload["persistFileName"].template get<std::string>();
            lotuskit::util::fs::writeFile(src, srcLen, path.c_str());
        }

    }

} // ns
