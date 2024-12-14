#pragma once

#include "structs/bbBlackboard.hpp"
#include "structs/engineActor.hpp"

namespace engine {

namespace event {

class EventMgr;

class ActorLink : public actor::ActorBaseLink {
public:
    void checkDerivedRuntimeTypeInfo(void*) const override;
    void* getRuntimeTypeInfo() const override;
};

struct EventRequestArg {
    EventRequestArg(const sead::SafeString& event) {
        // since we don't have an implementation for FixedSafeString constructors, we will manually copy them here
        auto len = strnlen(event.cstr(), 128);
        strncpy(event_name.getBuffer(), event.cstr(), len + 1 > 128 ? 128 : len + 1);
        *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(&event_name) + 0x8) = len + 1 > 128 ? 128 : len + 1;
    }

    bb::InitInfo<32> blackboard_info;
    sead::Matrix34f _1918;
    int start_frame = -1;
    u16 _194c;
    u8 _194e;
    u8 _194f;
    ActorLink event_starter_link;
    ActorLink _1968;
    sead::FixedSafeString<256> _1980;
    sead::FixedSafeString<128> event_name;
};
static_assert(sizeof(EventRequestArg) == 0x1b20);

} // namespace event

} // namespace engine
