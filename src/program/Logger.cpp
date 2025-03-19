#include "Logger.hpp"
#include "server/WebSocket.hpp"
//#include <thread/seadThread.h>
using json = nlohmann::json;

namespace lotuskit {
    // TODO filters/flags/??? for log outputs (svcOutputDebugString, ws, perhaps textwriter)
    void Logger::logJson(json obj, const std::string& ns, bool doBlocking, bool doDebugLog) {
        //sead::ThreadMgr* mgr = *EXL_SYM_RESOLVE<sead::ThreadMgr**>("sead::ThreadMgr::sInstance");
        //sead::Thread* thread = mgr->getCurrentThread(); // XXX what happens if we invoke this from a non-sead thread?

        // note these are kv pairs, ie json {msg:"msg", sys:{ns:"", tick:420, ...}}
        obj["sys"] = json::object({
            {"ns", ns},
            {"tick", svcGetSystemTick()}
            //{"tid", thread->getId()},
            //{"tname", thread->getName().cstr()} // TODO INamable+SafeString overloads?
        });
        std::string out = obj.dump();

        if (doBlocking) {
            if (doDebugLog) {
                svcOutputDebugString(out.c_str(), out.size());
            }
            lotuskit::server::WebSocket::sendTextBlocking(out.c_str());
        } else {
            if (doDebugLog) {
                svcOutputDebugString(out.c_str(), out.size());
            }
            lotuskit::server::WebSocket::sendTextNoblock(out.c_str());
        }
    }

    void Logger::logText(const std::string& msg, const std::string& ns, bool doBlocking, bool doDebugLog) {
        return Logger::logJson(json::object({ {"msg", msg} }), ns, doBlocking, doDebugLog);
    }

    /*
    void Logger::logf(LoggerNS ns, const char *fmt, ...) {
        // log with formatting pass
        va_list args;
        va_start(args, fmt);
        this->vlogf(ns, fmt, args);
        va_end(args);
    }

    void Logger::vlogf(LoggerNS ns, const char *fmt, va_list args) {
        // log with formatting pass
        char log_buffer[0x1000];
        if (nn::util::VSNPrintf(log_buffer, sizeof(log_buffer), fmt, args) > 0) {
            this->log(ns, log_buffer);
        }
    }
    */

} // ns
