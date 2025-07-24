#pragma once
#include "structs/engineEvent.hpp"
#include "util/alloc.hpp"
using String = lotuskit::String;


namespace lotuskit::util::event {

    void requestSimple(const String &eventName, engine::actor::ActorBase* eventStarterActor = nullptr);

} // ns

