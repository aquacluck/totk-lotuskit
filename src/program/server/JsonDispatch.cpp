#include <string>
#include "script/schedule.hpp"
#include "server/JsonDispatch.hpp"
#include "util/fs.hpp"
#include "exlaunch.hpp"
#include <lib/base64.hpp>
#include <lib/json.hpp>
using json = nlohmann::json;


namespace lotuskit::server {
    inline static nn::fs::FileHandle chunkedUploadFd = {};

    void JsonDispatch::dispatchText(const char* jsonText) {
        json jsonPayload = json::parse(jsonText);
        if (jsonPayload.contains("execScriptString")) {
            auto moduleName = jsonPayload.contains("execScriptModule") ? (jsonPayload["execScriptModule"].template get<std::string>()) : "web_anon.as";
            auto scriptText = jsonPayload["execScriptString"].template get<std::string>();
            lotuskit::script::schedule::tas::pushExecTextModule(moduleName.c_str(), moduleName.c_str(), scriptText.c_str(), "void main()");

        } else if (jsonPayload.contains("persistFileString") && jsonPayload.contains("persistFileName")) {
            auto src = jsonPayload["persistFileString"].template get<std::string>();
            auto path = jsonPayload["persistFileName"].template get<std::string>();
            lotuskit::util::fs::writeTextFile(src.c_str(), path.c_str());

        } else if (jsonPayload.contains("persistFileBinary") && jsonPayload.contains("persistFileName")) {
            std::string src = websocketpp::base64_decode(jsonPayload["persistFileBinary"].template get<std::string>()); // yes it returns a binary std::string
            // XXX how to free jsonPayload["persistFileBinary"] now?
            auto path = jsonPayload["persistFileName"].template get<std::string>();
            lotuskit::util::fs::writeFile(src.c_str(), src.size(), path.c_str());

        } else if (jsonPayload.contains("persistFileBinaryChunk") && jsonPayload.contains("persistFileName") && jsonPayload.contains("persistChunkOp") && jsonPayload.contains("persistChunkOffset")) {
            // write a file split across several ws frames -- gross hack
            std::string src = websocketpp::base64_decode(jsonPayload["persistFileBinaryChunk"].template get<std::string>()); // yes it returns a binary std::string
            auto path = jsonPayload["persistFileName"].template get<std::string>();
            auto chunkOp = jsonPayload["persistChunkOp"].template get<u32>();
            auto chunkOffset = jsonPayload["persistChunkOffset"].template get<u32>();
            lotuskit::util::fs::writeFileChunked(&chunkedUploadFd, src.c_str(), src.size(), path.c_str(), chunkOp, chunkOffset);
        }
    }

} // ns
