#pragma once
#include "lib.hpp"
#include "nn/util.h"

namespace lotuskit::server::SocketThread {
    constexpr size_t STACK_SIZE = 0x2000; // XXX idk
    alignas(PAGE_SIZE) u8 bindStack[STACK_SIZE];
    alignas(PAGE_SIZE) u8 sendStack[STACK_SIZE];

    // handles incoming connections. does not recv, see RecvNoblock for that
    void ThreadFunc_Bind(void* _) {
        char buf[200];
        while (true) {
            nn::util::SNPrintf(buf, sizeof(buf), "woo %d", 420);
            svcOutputDebugString(buf, strlen(buf));
            svcSleepThread(1000000000); // 1B ns = 1s
            //nn::os::SleepThread(nn::os::ConvertToTimeSpan(1000000000));
        }
    }

    // sends queued/deferred log messages. See SendBlock to bypass this and send immediately, eg to get a log out before a known crash
    void ThreadFunc_Send(void* _) {
        char buf[200];
        while (true) {
            nn::util::SNPrintf(buf, sizeof(buf), "hoo %d", 420);
            svcOutputDebugString(buf, strlen(buf));
            svcSleepThread(1000000000); // 1B ns = 1s
            //nn::os::SleepThread(nn::os::ConvertToTimeSpan(1000000000));
        }
    }

    void RecvNoblock() {
    }

    void SendBlock() {
    }

    void CreateAndStart() {
        constexpr auto PRIO = 55;
        constexpr auto CORE_ID = 2;

        Handle bindHandle;
        svcCreateThread(&bindHandle, (void*)&ThreadFunc_Bind, nullptr, bindStack+STACK_SIZE, PRIO, CORE_ID);
        svcStartThread(bindHandle);

        Handle sendHandle;
        svcCreateThread(&sendHandle, (void*)&ThreadFunc_Send, nullptr, sendStack+STACK_SIZE, PRIO, CORE_ID);
        svcStartThread(sendHandle);
    }

} // ns
