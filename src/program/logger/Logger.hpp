#pragma once

#include <cstdarg>
#include <cstring>
#include "nn/result.hpp"
#include "structs.hpp"


// TODO eventually keep track here in the backend of which ns have active subscriptions and drop the rest:
//      however we store+present the actual feature flagging ui, subscriptions should be the source of truth for what we currently need on the frontend.
//      Keeping the mod in frame sync is a goal, I want to be able to drive tas without worrying about scheduling oopsies eating frames.
//      So keeping the pubsub noise down (and ideally any pointless work at ns publisher callsites too) will remain important, it's blocking the game.
// XXX any point merging immediately near socket::Send() calls together? syscalls, syn/acks, idk

// Lets the frontend do a routing/subscription thingy for unsolicited structured data. Max 32 length for now
typedef const char* LoggerNS;
constexpr LoggerNS NS_DEFAULT_TEXT = "DEFAULT_TEXT";
constexpr LoggerNS NS_ACTOR_PLAYER = "ACTOR_PLAYER";
constexpr LoggerNS NS_ACTOR_PLAYER_RELATIONS = "ACTOR_PLAYER_RELATIONS";
constexpr LoggerNS NS_ACTOR = "ACTOR";
constexpr LoggerNS NS_SAVESTATE = "SAVESTATE";
constexpr LoggerNS NS_COMMAND = "COMMAND";
constexpr LoggerNS NS_VFRMGR = "VFRMGR";
constexpr LoggerNS NS_BACKEND_CONFIG = "BACKEND_CONFIG";
constexpr LoggerNS NS_PHYSICS_SPAM = "PHYSICS_SPAM";
constexpr LoggerNS DoDebugLog_LogNS[] = {
    // only constexpr ns will ever be debug logged -- but dynamic ns can still pass to frontend
    NS_DEFAULT_TEXT,
    NS_SAVESTATE,
    NS_COMMAND,
    NS_BACKEND_CONFIG,
    NS_ACTOR,
    "", // XXX sizeof LoggerNS doesn't work, idk how2cpp lol. using std stuff complains about allocators even here with constexpr? idc about allocation, shouldnt it be compiled away? limit ~100 static buf
};
constexpr bool do_svcOutputDebugString_log(LoggerNS ns_str) {
    for (auto i=0; true; i++) {
        if (strcmp("", DoDebugLog_LogNS[i]) == 0) {
            return false;
        }
        if (strcmp(ns_str, DoDebugLog_LogNS[i]) == 0) {
            return true;
        }
    }
};


enum class LoggerState {
    UNINITIALIZED = 0,
    CONNECTED = 1,
    UNAVAILABLE = 2,
    DISCONNECTED = 3
};

class Logger {
public:
    //Logger() = default;
    void init(const char* ip);

    bool mDoOpenSocket;
    bool mDoLogSocket;
    bool mDoHackCommandSocket;

    LoggerState mState;
    int mSocketFd;

    void log(LoggerNS ns, const char *payload);
    void logf(LoggerNS ns, const char *fmt, ...);
    void vlogf(LoggerNS ns, const char *fmt, va_list vlist);
    void tryRecvCommandMainLoop();
};



class ModCommand_Savestate_ActorPos {
public:
    bool is_active_request;
    char actor_selector_named[33];
    Vector3f pos32;
    Vector3d pos64;
    Matrix33f rot;
};



// random socket enums
// TODO throw these in nn socket header or something, idk where they belong

// ams::socket::Type https://github.com/Atmosphere-NX/Atmosphere/blob/master/libraries/libstratosphere/include/stratosphere/socket/socket_types.hpp
enum class SocketType : s32 {
    SOCK_DEFAULT   = 0,
    SOCK_STREAM    = 1,
    SOCK_DGRAM     = 2,
    SOCK_RAW       = 3,
    SOCK_SEQPACKET = 5,
    SOCK_NONBLOCK  = 0x20000000,
};
enum class SocketProtocol : s32 {
    IPPROTO_IP      = 0,
    IPPROTO_ICMP    = 1,
    IPPROTO_TCP     = 6,
    IPPROTO_UDP     = 17,
    IPPROTO_NONE    = 59,
    IPPROTO_UDPLITE = 136,
    IPPROTO_RAW     = 255,
    IPPROTO_MAX     = 256,
};
// https://github.com/Atmosphere-NX/Atmosphere/blob/master/libraries/libstratosphere/include/stratosphere/socket/socket_options.hpp
enum class SocketLevel : s32 {
      Sol_Ip      =   0,
      Sol_Icmp    =   1,
      Sol_Tcp     =   6,
      Sol_Udp     =  17,
      Sol_UdpLite = 136,

      Sol_Socket  = 0xFFFF,
  };

enum class SocketOption : u32 {
    So_Debug              = (1 <<  0),
    So_AcceptConn         = (1 <<  1),
    So_ReuseAddr          = (1 <<  2),
    So_KeepAlive          = (1 <<  3),
    So_DontRoute          = (1 <<  4),
    So_Broadcast          = (1 <<  5),
    So_UseLoopback        = (1 <<  6),
    So_Linger             = (1 <<  7),
    So_OobInline          = (1 <<  8),
    So_ReusePort          = (1 <<  9),

    So_SndBuf             = (1 << 12) | 0x01,
    So_RcvBuf             = (1 << 12) | 0x02,
    So_SndLoWat           = (1 << 12) | 0x03,
    So_RcvLoWat           = (1 << 12) | 0x04,
    So_SndTimeo           = (1 << 12) | 0x05,
    So_RcvTimeo           = (1 << 12) | 0x06,
    So_Error              = (1 << 12) | 0x07,
    So_Type               = (1 << 12) | 0x08,
    So_Label              = (1 << 12) | 0x09,
    So_PeerLabel          = (1 << 12) | 0x10,
    So_ListenQLimit       = (1 << 12) | 0x11,
    So_ListenQLen         = (1 << 12) | 0x12,
    So_ListenIncQLen      = (1 << 12) | 0x13,
    So_SetFib             = (1 << 12) | 0x14,
    So_User_Cookie        = (1 << 12) | 0x15,
    So_Protocol           = (1 << 12) | 0x16,

    So_Nn_Shutdown_Exempt = (1 << 16),

    So_Vendor             = (1u << 31),
    So_Nn_Linger          = So_Vendor | 0x01,

    // ====================================

    Ip_Options                =  1,
    Ip_HdrIncl                =  2,
    Ip_Tos                    =  3,
    Ip_Ttl                    =  4,
    Ip_RecvOpts               =  5,
    Ip_Multicast_If           =  9,
    Ip_Multicast_Ttl          = 10,
    Ip_Multicast_Loop         = 11,
    Ip_Add_Membership         = 12,
    Ip_Drop_Membership        = 13,
    Ip_Multicast_Vif          = 14,
    Ip_Rsvp_On                = 15,
    Ip_Rsvp_Off               = 16,
    Ip_Rsvp_Vif_On            = 17,
    Ip_Rsvp_Vif_Off           = 18,
    Ip_PortRange              = 19,
    Ip_Faith                  = 22,
    Ip_OnesBcast              = 23,
    Ip_BindAny                = 24,

    Ip_RecvTtl                = 65,
    Ip_MinTtl                 = 66,
    Ip_DontFrag               = 67,
    Ip_RecvTos                = 68,

    Ip_Add_Source_Membership  = 70,
    Ip_Drop_Source_Membership = 71,
    Ip_Block_Source           = 72,
    Ip_Unblock_Source         = 73,

    // ====================================

    Tcp_NoDelay    = (1 <<  0),
    Tcp_MaxSeg     = (1 <<  1),
    Tcp_NoPush     = (1 <<  2),
    Tcp_NoOpt      = (1 <<  3),
    Tcp_Md5Sig     = (1 <<  4),
    Tcp_Info       = (1 <<  5),
    Tcp_Congestion = (1 <<  6),
    Tcp_KeepInit   = (1 <<  7),
    Tcp_KeepIdle   = (1 <<  8),
    Tcp_KeepIntvl  = (1 <<  9),
    Tcp_KeepCnt    = (1 << 10),

    Tcp_Vendor     = So_Vendor,
};

// https://github.com/Atmosphere-NX/Atmosphere/blob/master/libraries/libstratosphere/include/stratosphere/socket/socket_errno.hpp
enum class SocketErrno : u32 {
    ESuccess        =   0,
    EPerm           =   1,
    ENoEnt          =   2,
    ESrch           =   3,
    EIntr           =   4,
    EIo             =   5,
    ENxIo           =   6,
    E2Big           =   7,
    ENoExec         =   8,
    EBadf           =   9,
    EChild          =  10,
    EAgain          =  11,
    EWouldBlock     =  11, //EAgain,
    ENoMem          =  12,
    EAcces          =  13,
    EFault          =  14,
    ENotBlk         =  15,
    EBusy           =  16,
    EExist          =  17,
    EXDev           =  18,
    ENoDev          =  19,
    ENotDir         =  20,
    EIsDir          =  21,
    EInval          =  22,
    ENFile          =  23,
    EMFile          =  24,
    ENotTy          =  25,
    ETxtBsy         =  26,
    EFBig           =  27,
    ENoSpc          =  28,
    ESPipe          =  29,
    ERofs           =  30,
    EMLink          =  31,
    EPipe           =  32,
    EDom            =  33,
    ERange          =  34,
    EDeadLk         =  35,
    EDeadLock       =  35, //EDeadLk,
    ENameTooLong    =  36,
    ENoLck          =  37,
    ENoSys          =  38,
    ENotEmpty       =  39,
    ELoop           =  40,
    ENoMsg          =  42,
    EIdrm           =  43,
    EChrng          =  44,
    EL2NSync        =  45,
    EL3Hlt          =  46,
    EL3Rst          =  47,
    ELnrng          =  48,
    EUnatch         =  49,
    ENoCsi          =  50,
    EL2Hlt          =  51,
    EBade           =  52,
    EBadr           =  53,
    EXFull          =  54,
    ENoAno          =  55,
    EBadRqc         =  56,
    EBadSsl         =  57,
    EBFont          =  59,
    ENoStr          =  60,
    ENoData         =  61,
    ETime           =  62,
    ENoSr           =  63,
    ENoNet          =  64,
    ENoPkg          =  65,
    ERemote         =  66,
    ENoLink         =  67,
    EAdv            =  68,
    ESrmnt          =  69,
    EComm           =  70,
    EProto          =  71,
    EMultiHop       =  72,
    EDotDot         =  73,
    EBadMsg         =  74,
    EOverflow       =  75,
    ENotUnuq        =  76,
    EBadFd          =  77,
    ERemChg         =  78,
    ELibAcc         =  79,
    ELibBad         =  80,
    ELibScn         =  81,
    ELibMax         =  82,
    ELibExec        =  83,
    EIlSeq          =  84,
    ERestart        =  85,
    EStrPipe        =  86,
    EUsers          =  87,
    ENotSock        =  88,
    EDestAddrReq    =  89,
    EMsgSize        =  90,
    EPrototype      =  91,
    ENoProtoOpt     =  92,
    EProtoNoSupport =  93,
    ESocktNoSupport =  94,
    EOpNotSupp      =  95,
    ENotSup         =  95, //EOpNotSupp,
    EPfNoSupport    =  96,
    EAfNoSupport    =  97,
    EAddrInUse      =  98,
    EAddrNotAvail   =  99,
    ENetDown        = 100,
    ENetUnreach     = 101,
    ENetReset       = 102,
    EConnAborted    = 103,
    EConnReset      = 104,
    ENoBufs         = 105,
    EIsConn         = 106,
    ENotConn        = 107,
    EShutDown       = 108,
    ETooManyRefs    = 109,
    ETimedOut       = 110,
    EConnRefused    = 111,
    EHostDown       = 112,
    EHostUnreach    = 113,
    EAlready        = 114,
    EInProgress     = 115,
    EStale          = 116,
    EUClean         = 117,
    ENotNam         = 118,
    ENAvail         = 119,
    EIsNam          = 120,
    ERemoteIo       = 121,
    EDQuot          = 122,
    ENoMedium       = 123,
    EMediumType     = 124,
    ECanceled       = 125,
    ENoKey          = 126,
    EKeyExpired     = 127,
    EKeyRevoked     = 128,
    EKeyRejected    = 129,
    EOwnerDead      = 130,
    ENotRecoverable = 131,
    ERfKill         = 132,
    EHwPoison       = 133,
    // ...
    EProcLim        = 156,
};

enum class SocketMsgFlag : s32 {
    Msg_None      = (0 << 0),

    Msg_Oob       = (1 << 0),
    Msg_Peek      = (1 << 1),
    Msg_DontRoute = (1 << 2),

    Msg_Trunc     = (1 << 4),
    Msg_CTrunc    = (1 << 5),
    Msg_WaitAll   = (1 << 6),
    Msg_DontWait  = (1 << 7),
};
