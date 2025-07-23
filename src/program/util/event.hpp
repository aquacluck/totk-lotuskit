#pragma once
#include <string>
#include "structs/engineEvent.hpp"


namespace lotuskit::util::event {

    void requestSimple(const String &eventName, engine::actor::ActorBase* eventStarterActor = nullptr);

} // ns

