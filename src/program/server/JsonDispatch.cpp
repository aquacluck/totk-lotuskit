#include <string>
#include "angelscript.h"
#include "script/engine.hpp"
#include "script/globals.hpp"
#include "server/JsonDispatch.hpp"
#include "lib.hpp"
#include "lib/json.hpp"
#include "nn/util.h"
using json = nlohmann::json;


namespace lotuskit::server {
    void testAngelScript(std::string scriptText) {
        // TODO extract a longer lived engine instance, route ws+tas through it
        //auto* engine = lotuskit::script::engine::CreateAndSetupEngine();
        AngelScript::asIScriptEngine *engine = AngelScript::asCreateScriptEngine();
        lotuskit::script::engine::configureEngine(engine);
        lotuskit::script::globals::registerGlobals(engine);

        auto* mod = lotuskit::script::engine::testBuildModule(engine, scriptText.c_str());
        const char* entryPoint = "void main()";
        lotuskit::script::engine::testExecFuncInNewCtx(engine, mod, entryPoint);

        engine->ShutDownAndRelease();
    }

    void JsonDispatch::dispatchText(const char* jsonText) {
        json jsonPayload = json::parse(jsonText);
        if (jsonPayload.contains("execScriptString")) {
            auto scriptText = jsonPayload["execScriptString"].template get<std::string>();
            testAngelScript(scriptText);
        }
    }

} // ns
