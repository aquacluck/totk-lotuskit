#include "Logger.hpp"
#include "server/WebSocket.hpp"
#include "thread/seadThread.h"
#include "sym/thread/seadThread.h"
using json = nlohmann::json;

namespace lotuskit {
    // TODO filters/flags/??? for log outputs (svcOutputDebugString, ws, perhaps textwriter)
    void Logger::logJson(json obj, const std::string& ns, bool doBlocking) {
        sead::ThreadMgr* mgr = *exl::util::pointer_path::FollowSafe<sead::ThreadMgr*, sym::sead::ThreadMgr::sInstance>();
        sead::Thread* thread = mgr->getCurrentThread(); // XXX what happens if we invoke this from a non-sead thread?

        // note these are kv pairs, ie json {msg:"msg", sys:{ns:"", tick:420, ...}}
        obj["sys"] = json::object({
            {"ns", ns},
            {"tick", svcGetSystemTick()},
            {"tid", thread->getId()},
            {"tname", thread->getName().cstr()} // TODO INamable+SafeString overloads?
        });
        std::string out = obj.dump();

        if (doBlocking) {
            svcOutputDebugString(out.c_str(), out.size());
            lotuskit::server::WebSocket::SendTextBlocking(out.c_str());
        } else {
            svcOutputDebugString(out.c_str(), out.size());
            lotuskit::server::WebSocket::SendTextNoblock(out.c_str());
        }
    }

    void Logger::logText(const std::string& msg, const std::string& ns, bool doBlocking) {
        return Logger::logJson(json::object({ {"msg", msg} }), ns, doBlocking);
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
