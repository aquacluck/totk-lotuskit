#include "Logger.hpp"
#include "server/WebSocket.hpp"
#include "util/fs.hpp"
#include <nn/fs.h>
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

    void Logger::dumpTextFileIntoNS(const std::string& filename, const std::string& ns) {
        constexpr size_t maxOut = 0x2000;
        char out[maxOut];
        lotuskit::util::fs::readTextFile(out, maxOut, filename.c_str());
        Logger::logText(out, ns, false, false);
        Logger::logJson(json::object({
            {"endDump", true} // announce
        }), ns, false, false);
    }

    void Logger::dumpDirectoryIndexIntoNS(const std::string& path, const std::string& ns) {
        const auto indexMode = nn::fs::OpenDirectoryMode::OpenDirectoryMode_All; // subdirs+files
        static const char* ns_;
        ns_ = ns.c_str(); // HACK no capture

        lotuskit::util::fs::visitDirectoryEntries(path, indexMode, [](const std::string& cpath, nn::fs::DirectoryEntry* dentry) {
            Logger::logJson(json::object({
                {"name", dentry->mName},
                {"size", dentry->mFileSize},
                {"type", dentry->mTypeByte}
            }), ns_, false, false);
        });

        Logger::logJson(json::object({
            {"endDump", true} // announce
        }), ns, false, false);
    }

} // ns
