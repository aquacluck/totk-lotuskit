#pragma once
#include "lib.hpp"
#include "structs/bbBlackboard.hpp"
#include "structs/engineActor.hpp"
#include <prim/seadSafeString.h>

namespace lotuskit::util::actor {
    using CreateFunc = bool (engine::actor::ActorInstanceMgr*, const sead::SafeString&, const engine::actor::ActorMgr::CreateArg&,
                             engine::actor::CreateWatcherRef*, engine::actor::CreatePriority, engine::actor::PreActor*,
                             engine::actor::ActorFile*, sead::Function*, bool, engine::actor::ActorMgr::Result*, engine::actor::PreActor**);

    void testCreate();

} // ns
