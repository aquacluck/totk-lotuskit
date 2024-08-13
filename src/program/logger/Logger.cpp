#include "Logger.hpp"
#include "nn/socket.h"
#include "nn/socket.hpp"
#include "nn/nifm.hpp"
#include "nn/util.hpp"
#include "lib.hpp"

constexpr inline auto AF_INET = 2; // XXX domain ams::socket::Family::Af_Inet (ipv4)
constexpr inline u32 ip4_any = 0x00'00'00'00; // InAddr_Any
constexpr inline u32 ip4_localhost = 0x7f'00'00'01; // InAddr_Loopback

constexpr auto SOCKET_POOL_SIZE = 0x600000;
constexpr auto ALLOCATOR_POOL_SIZE = 0x20000;
static u8 s_socketPool[SOCKET_POOL_SIZE + ALLOCATOR_POOL_SIZE] __attribute__((aligned(0x4000)));
static u32 s_dgram_uniq_id_ctr = 0x00000000; // counter for all unsolicited messages we send

// instead of connect
#define DO_SOCKET_LISTEN 0

ModCommand_Savestate_ActorPos g_ModCommand_Savestate_ActorPos[5];


class LoggerTransport {
    public:
    static s32 send_dgram(int socket_fd, bool do_http, u32 uniq_id, const char* ns, const char* payload) {
        // payload should be a json literal ready for the wire -- no binary
        char wire_buffer[0x1000];
        const u16 payload_len = strlen(payload);
        u16 expected_wire_len = 0;

        // count all the non-payload dgram/http_resp_body chars in order to calculate content-length from known payload length:
        // [0x00000000, "32", 0x0420, payload]
        // 12345678901234  56789012345       6
        constexpr auto dgram_overhead = 26 + 32; // yes really
        const auto dgram_len = payload_len + dgram_overhead;

        if (do_http) {
            constexpr auto http_header_len = 44;                               //    DDDDDD => +6 = 44
            //                                   12345678901234567 8 9012345678901234    5 6 7 8
            const char* http_dgram_format_str = "HTTP/1.0 200   OK\r\nContent-Length:%6d\r\n\r\n[0x%08x, \"%32s\", 0x%04x, %s]";
            nn::util::SNPrintf(wire_buffer, sizeof(wire_buffer), http_dgram_format_str, dgram_len, uniq_id, ns, payload_len, payload);
            expected_wire_len = http_header_len + dgram_len;

        } else {
            // json literals over socket. BackendProxy is hardcoded to parse these using the length field, but all this could be ws instead
            const char* dgram_format_str = "[0x%08x, \"%32s\", 0x%04x, %s]";
            //                              uniq_id,     ns,      len, payload
            nn::util::SNPrintf(wire_buffer, sizeof(wire_buffer), dgram_format_str, uniq_id, ns, payload_len, payload);
            expected_wire_len = dgram_len;
        }

        const auto wire_len = strlen(wire_buffer);
        if (wire_len != expected_wire_len) {
            nn::util::SNPrintf(wire_buffer, sizeof(wire_buffer), "[ERROR] socket::Send() preflight len %i wanted %i %s ", wire_len, expected_wire_len, payload);
            svcOutputDebugString(wire_buffer, strlen(wire_buffer));
            return -1;
        }

        s32 sent_bytes = nn::socket::Send(socket_fd, wire_buffer, wire_len, 0); // blocking

        if (sent_bytes == expected_wire_len) {
            return sent_bytes; // Success

        } else if (sent_bytes == -1) {
            nn::util::SNPrintf(wire_buffer, sizeof(wire_buffer), "[ERROR] socket::Send() ret -1");
            svcOutputDebugString(wire_buffer, strlen(wire_buffer));
            return sent_bytes;

        } else {
            nn::util::SNPrintf(wire_buffer, sizeof(wire_buffer), "[ERROR] socket::Send() ret %i wanted %i %s", sent_bytes, expected_wire_len, payload);
            svcOutputDebugString(wire_buffer, strlen(wire_buffer));
            return sent_bytes;
        }
    }
    // trySocketInit, trySocketMaintain, idk in statics here? keep transport out of logging logic
};


void Logger::init(const char* ip) {
    if (!mDoOpenSocket) {
        return;
    }
    if (mState != LoggerState::UNINITIALIZED) {
        return;
    }

    // TODO its unclear whether nifm calls are necessary (maybe this can init radios or detect airplane mode? idk), but regardless we need to extract this into:
    // - trySocketListen, called on a throttle somewhere in mod main loop
    // - keybind/command to enable/disable/trigger trySocketListen (offline mode)
    // - keybind/command to toggle blocking/nonblocking behaviors (closer syncing vs playable performance)
    s32 sock_errno = 0;
    nn::nifm::Initialize();
    nn::socket::Initialize(s_socketPool, SOCKET_POOL_SIZE, ALLOCATOR_POOL_SIZE, 14);
    nn::nifm::SubmitNetworkRequest();
    while (nn::nifm::IsNetworkRequestOnHold()) {}
    if (!nn::nifm::IsNetworkAvailable()) {
        mState = LoggerState::UNAVAILABLE;
        this->log(NS_DEFAULT_TEXT, R"("logger: nifm::IsNetworkAvailable() falsy")");
        return;
    }

    // Open socket
    if ((mSocketFd = nn::socket::Socket(AF_INET, (s32)(SocketType::SOCK_STREAM), (s32)(SocketProtocol::IPPROTO_TCP))) < 0) {
        mState = LoggerState::UNAVAILABLE;
        this->logf(NS_DEFAULT_TEXT, R"("logger: socket::Socket() errno %i")", mSocketFd);
        return;
    }

    const u16 bind_port = 7072;

    if (DO_SOCKET_LISTEN) {
        // Set keepalive
        int flags = 1; // XXX what is this
        nn::socket::SetSockOpt(mSocketFd, (s32)(SocketLevel::Sol_Socket), (u32)(SocketOption::So_KeepAlive), &flags, sizeof(flags));

        // TODO maybe keybind, or detect emulators -> localhost only?
        // Unless we add a config file or listen on lan subnet or etc, toggling InAddr_Any is the simplest option.
        // But dont do this inside emulators, some possibility of emu escape / host rce / idk
        //const u32 bind_ip = ip4_localhost;

        //sockaddr_in serverAddress;
        //serverAddress.sin_family = AF_INET;
        //serverAddress.sin_addr.s_addr = bind_ip;
        //serverAddress.sin_port = nn::socket::InetHtons(bind_port);

        const char* ip = "0.0.0.0";
        //const char* ip = "127.0.0.1";
        in_addr hostAddress = {0};
        nn::socket::InetAton(ip, &hostAddress);

        sockaddr serverAddress = {0};
        serverAddress.family = AF_INET;
        //serverAddress.address.s_addr = bind_ip;
        serverAddress.address = hostAddress;
        serverAddress.port = nn::socket::InetHtons(bind_port);

        if ((sock_errno = nn::socket::Bind(mSocketFd, &serverAddress, sizeof(serverAddress))) != 0) {
            mState = LoggerState::UNAVAILABLE;
            this->logf(NS_DEFAULT_TEXT, R"("logger: socket::Bind() errno %i")", sock_errno);
            return;
        }
        if ((sock_errno = nn::socket::Listen(mSocketFd, 1)) != 0) {
            mState = LoggerState::UNAVAILABLE;
            this->logf(NS_DEFAULT_TEXT, R"("logger: socket::Listen() errno %i")", sock_errno);
            return;
        }

        u32 addressLen;
        this->logf(NS_DEFAULT_TEXT, R"(logger: socket::Accept() waiting on %i")", mSocketFd);
        if ((sock_errno = nn::socket::Accept(mSocketFd, &serverAddress, &addressLen)) != 0) { // blocking
            mState = LoggerState::DISCONNECTED;
            // FIXME errno 1 EPERM on windows ryujinx? if i ignore this unless < 0, Send() later fails...
            this->logf(NS_DEFAULT_TEXT, R"(logger: socket::Accept() errno %i")", sock_errno);
            return;
        }
    } else {

        // Do socket::connect
        in_addr hostAddress = {0};
        nn::socket::InetAton(ip, &hostAddress);
        sockaddr serverAddress = {0};
        serverAddress.address = hostAddress;
        serverAddress.port = nn::socket::InetHtons(bind_port);
        serverAddress.family = AF_INET;

        if ((sock_errno = nn::socket::Connect(mSocketFd, &serverAddress, sizeof(serverAddress))) != 0) {
            this->logf(NS_DEFAULT_TEXT, R"("logger: socket::Connect() errno %i")", sock_errno);
            return;
        }
    }

    mState = LoggerState::CONNECTED;
    this->log(NS_BACKEND_CONFIG, R"({"backend_is_connected": true, "msg": "logger socket connected"})");

    /*
    // TODO extract to json_array_printf or etc
    constexpr auto bufsize = 3200;
    char buf[bufsize];
    char* buf_tail = buf;
    *(buf_tail++) = '['; // begin array
    for (auto i=0; true; i++) {
        auto item = DoDebugLog_LogNS[i];
        auto item_len = strlen(item);
        if (item_len == 0) { break; } // XXX empty string terminates this particular list
        *(buf_tail++) = '"'; // begin string literal
        memcpy(buf_tail, item, item_len);
        buf_tail += item_len;
        *(buf_tail++) = '"'; // end string literal
        *(buf_tail++) = ',';
        *(buf_tail++) = ' ';
    }
    buf_tail[-2] = ']'; // end array in place of trailing comma
    buf_tail[-1] = '\0'; // terminate in place of space

    // report readonly ns filter
    this->logf(NS_BACKEND_CONFIG, R"({"svcOutputDebugString_log_ns_passlist": %s})", buf);
    */
}


void Logger::log(LoggerNS ns, const char *payload) {
    // send a json payload on given ns, to console + socket log

    if (do_svcOutputDebugString_log(ns)) {
        svcOutputDebugString(payload, strlen(payload));
    }

    if (mDoLogSocket && mState == LoggerState::CONNECTED) {
        // TODO any timing/thread/etc info like svcOutputDebugString?
        u32 uniq_id = ++s_dgram_uniq_id_ctr;
        constexpr bool do_http = false;

        // TODO if (do_socketdebug_log(ns))
        LoggerTransport::send_dgram(mSocketFd, do_http, uniq_id, (const char*)ns, payload);
    }
}

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

void Logger::tryRecvCommandMainLoop() {
    if (!mDoHackCommandSocket || mState != LoggerState::CONNECTED) {
        return;
    }

    // only very simple http requests coming in, read a query
    char query_buffer[0x500];
    auto flags = (s32)(SocketMsgFlag::Msg_DontWait); // sort of like per-recv nonblock
    // XXX if we never go over 0x500 for a command, and we only check this socket every frame and its all local, we're usually going to get full commands. but no guarantee, look for \r\n\r\n ideally
    s32 recv_size = nn::socket::Recv(mSocketFd, query_buffer, 0x500-3, flags);
    if (recv_size == -1) {
        // XXX wheres the errno so we can see EWOULDBLOCK? lets just assume its EWOULDBLOCK
        return;
    }
    query_buffer[recv_size] = '\0'; // -3 buffer for when i dont wanna think about off by one


    // Route query to any command, maybe yield a reponse
    // TODO defer dgram responses when needed, so we can return async results another frame. Not everything will be observable the same frame we poke it. keep track of outstanding uniq_id+command, return async results with it
    // XXX this sscanf business might be insecure?

    u32 uniq_id = 0;

    constexpr auto savestate_paste_simple = "POST /backend/savestate/paste_simple?";
    if (strncmp(savestate_paste_simple, query_buffer, strlen(savestate_paste_simple)) == 0) {
        char target[33];
        char prop1[33]; Vector3f pos32;
        char prop2[33]; Vector3d pos64;
        constexpr auto fmt = "POST /backend/savestate/paste_simple?uniq_id=0x%08X %s %s %f %f %f %s %lf %lf %lf";

        if (sscanf(query_buffer, fmt, &uniq_id, target, prop1, &pos32.X, &pos32.Y, &pos32.Z, prop2, &pos64.X, &pos64.Y, &pos64.Z) == 10) {
            // XXX only accept hardcoded format
            if (strcmp(target, "Player") != 0) {
                LoggerTransport::send_dgram(mSocketFd, false, uniq_id, NS_COMMAND, R"({"err": "savestate aselector"})");
                return; // fail
            }
            if (strcmp(prop1, "pos32") != 0) {
                LoggerTransport::send_dgram(mSocketFd, false, uniq_id, NS_COMMAND, R"({"err": "savestate prop1 != pos32"})");
                return; // fail
            }
            if (strcmp(prop2, "pos64") != 0) {
                LoggerTransport::send_dgram(mSocketFd, false, uniq_id, NS_COMMAND, R"({"err": "savestate prop2 != pos64"})");
                return; // fail
            }

            // XXX how 2 request format for copy/pasting multi actor+prop savestates? how 2 store the resulting mess in cpp?
            // {"actor_copy": ["Player", arg2, selection3], props: [["pos64", "pos32", "rot"]], dest_slot: 10 }
            // {"actor_paste": ["Player", arg2, "Weapon_Shield_003"], props: [["pos64", "pos32", "rot"], ["pos64", "frozen"], ["createondemand"]], src_slot: 12 }

            // TODO how to cmd classes, decent slot selection for concurrent commands
            auto &cmd = g_ModCommand_Savestate_ActorPos[0];
            cmd.is_active_request = true;
            strncpy(cmd.actor_selector_named, target, 33);
            cmd.pos32 = pos32;
            cmd.pos64 = pos64;
            cmd.rot.m11 = 0; cmd.rot.m12 = 1; cmd.rot.m13 = 0; cmd.rot.m21 = 0; cmd.rot.m22 = 0; cmd.rot.m23 = 0; cmd.rot.m31 = 0; cmd.rot.m32 = 0; cmd.rot.m33 = 0; // XXX get from query, or allow partial? convert to physics?

            LoggerTransport::send_dgram(mSocketFd, false, uniq_id, NS_COMMAND, R"({"msg": "queued warp"})");
            return;
        }
    }

    constexpr bool do_http = false; // TODO just speak websocket and drop the http thing? its not that hard
    LoggerTransport::send_dgram(mSocketFd, do_http, uniq_id, NS_COMMAND, R"({"fail": true})");
}

