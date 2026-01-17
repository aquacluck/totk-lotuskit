#pragma once

#include <nn/os.h>
#include <heap/seadDisposer.h>
#include <container/seadListImpl.h>

namespace ai {

class AIMgr {
public:
    virtual ~AIMgr();

    void pulseTLS() {
        u32 value = nn::os::GetTlsValue(mTlsSlot);
        if (value & 0x80000000)
            nn::os::SetTlsValue(mTlsSlot, 0x80000001);
    }

private:
    void* mElementCreator;
    sead::ListNode mListNode;
    char _20[0x18];
    u32 mOutputSyncFlag;
    u16 mCalcOptions;
    char _3e[0x54 - 0x3e];
    nn::os::TlsSlot mTlsSlot;
    char _58[8];
    SEAD_SINGLETON_DISPOSER(AIMgr)
};
static_assert(sizeof(AIMgr) == 0x80);

} // namespace ai
