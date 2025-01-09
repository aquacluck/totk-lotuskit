#include <string>
#include "script/engine.hpp"
#include "server/JsonDispatch.hpp"
#include "exlaunch.hpp"
#include "lib/json.hpp"
using json = nlohmann::json;


namespace lotuskit::server {
    void JsonDispatch::dispatchText(const char* jsonText) {
        json jsonPayload = json::parse(jsonText);
        if (jsonPayload.contains("execScriptString")) {
            auto scriptText = jsonPayload["execScriptString"].template get<std::string>();
            lotuskit::script::engine::execInScratchModule(scriptText.c_str());
        }
    }

} // ns
