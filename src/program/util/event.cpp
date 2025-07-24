#include "exlaunch.hpp"
#include "script/globals.hpp"
#include "structs/engineActor.hpp"
#include "util/event.hpp"
#include "util/alloc.hpp"
using String = lotuskit::String;


namespace lotuskit::util::event {

    void requestSimple(const String &eventName, engine::actor::ActorBase* eventStarterActor) {
        engine::event::EventRequestArg reqArg{eventName.c_str()};

        if (eventStarterActor != nullptr) {
            using SetActorLinkFunc = bool (engine::actor::ActorBaseLink*, engine::actor::ActorBase*, u8);
            auto setLink = EXL_SYM_RESOLVE<SetActorLinkFunc*>("engine::actor::BaseProcLink::setProc");
            const u8 mask = 0; // third arg is some link reference type mask, usually is 0 but notably is 5 for new item get messages
            setLink(&reqArg.event_starter_link, eventStarterActor, mask); // bool return ignored
        }

        auto* eventMgr = *EXL_SYM_RESOLVE<engine::event::EventMgr**>("engine::event::EventMgr::sInstance");
        using RequestFunc = bool (engine::event::EventMgr*, const engine::event::EventRequestArg&);
        auto reqFunc = EXL_SYM_RESOLVE<RequestFunc*>("engine::event::EventMgr::requestEvent");
        reqFunc(eventMgr, reqArg); // returns false on fail
    }

} // ns

