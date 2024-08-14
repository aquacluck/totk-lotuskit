#include "lib.hpp"

#include "logger/Logger.hpp"
//#include "nn/socket.h"
//#include "nn/socket.hpp"
#include "nn/fs.hpp"
#include "nn/hid.hpp"
#include "helpers/InputHelper.hpp"

#include "nn/util.hpp"
#include "nn.hpp"
#include "hooks.hpp"


void read_file(char* out, const char* path, int max_len, const char* default_) {
    char errbuf[200];
    nn::fs::FileHandle fd;
    nn::Result res = nn::fs::OpenFile(&fd, path, nn::fs::OpenMode_Read);
    if (!res.isSuccess()) {
        strcpy(out, default_);
        nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] cant open file %s ", path);
        svcOutputDebugString(errbuf, strlen(errbuf));
        return;
    }

    long int fd_len = 0;
    res = nn::fs::GetFileSize(&fd_len, fd);
    if (!res.isSuccess()) {
        strcpy(out, default_);
        nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] cant get file size %s ", path);
        svcOutputDebugString(errbuf, strlen(errbuf));
        return;
    }
    if (fd_len > max_len) {
        strcpy(out, default_);
        nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] file contents too long %d > %d %s ", fd_len, max_len, path);
        svcOutputDebugString(errbuf, strlen(errbuf));
        return;
    }

    res = nn::fs::ReadFile(fd, 0, out, fd_len);
    if (!res.isSuccess()) {
        strcpy(out, default_);
        nn::util::SNPrintf(errbuf, sizeof(errbuf), "[ERROR] file read fail %s ", path);
        svcOutputDebugString(errbuf, strlen(errbuf));
        return;
    }

    nn::fs::CloseFile(fd);
}


HOOK_DEFINE_INLINE(nnMainHook) {
    static const ptrdiff_t s_offset = s_nnMain_preMainLoop;
    static void Callback(exl::hook::InlineCtx* ctx) {
        // socket init fails without some sdk stuff initialized first?
        // we let nnMain handle that, then steal execution right before it jumps into the game.

        // figure out where+when to connect
        char ip[16] = "";
        read_file(ip, "content:/totk_lotuskit/server_ip.txt", sizeof(ip), "127.0.0.1");

        char do_connect_on_bootup[2] = "1";
        read_file(do_connect_on_bootup, "content:/totk_lotuskit/do_connect_on_bootup.txt", sizeof(do_connect_on_bootup), "1");

        char do_connect_on_whistle[2] = "1";
        read_file(do_connect_on_whistle, "content:/totk_lotuskit/do_connect_on_whistle.txt", sizeof(do_connect_on_whistle), "1");
        if (do_connect_on_whistle[0] == '1') {
            LoggerConnectOnWhistleHook::Install();
        }

        char buf[200];
        nn::util::SNPrintf(buf, sizeof(buf), "lotuskit using ip4 addr %s, do_connect_on_bootup: %c, do_connect_on_whistle: %c", ip, do_connect_on_bootup[0], do_connect_on_whistle[0]);
        svcOutputDebugString(buf, strlen(buf));

        // init logger socket
        main_logger = new Logger(); // hooks.hpp
        main_logger->mState = LoggerState::UNINITIALIZED;
        main_logger->mDoOpenSocket = true; // prepare socket
        main_logger->mDoLogSocket = true; // actually send to socket
        main_logger->mDoHackCommandSocket = true; // also abuse logging socket to recv commands
        strcpy(main_logger->ip, ip);
        main_logger->init();
        if (do_connect_on_bootup[0] == '1') {
            main_logger->connect();
        }

        // TODO centralize on-connect info dump, or allow frontend to request it
        main_logger->logf(NS_DEFAULT_TEXT, R"("main_offset %p")", main_offset);

        InputHelper::initKBM();
        InputHelper::setPort(0); // default controller port
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();
    main_offset = exl::util::GetMainModuleInfo().m_Total.m_Start; // hooks.hpp

    nnMainHook::Install();
    ActorRelationAddHook::Install();
    ActorRelationRemoveHook::Install();
    WorldManagerModuleBaseProcHook::Install();
    TryGetPlayerPhysicsPosPtrHook::Install();

#if DO_XXX_ACTOR_CREATION_LOG
    TestCreateActorHook1::Install();
    TestCreateActorHook2::Install();
    TestCreateActorHook3::Install();
    TestCreateActorHook4::Install();
    TestCreateActorHook5::Install();
#endif
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
