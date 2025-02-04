#pragma once
#include "structs/exkingEnums.hpp"

namespace ares::thread { class OnThreadUnitWatcher_STUB { public: u8 idk[0x18]; }; }

namespace ares::resource {
    class ResourceUnit; // TODO

    class ResourceBinder {
        u8 mStateFlags;
        u8 mType;
        u8 idk[2];
        ares::resource::BinderStatus mStatus;
        ResourceUnit* mpResourceUnit;
        ares::thread::OnThreadUnitWatcher_STUB mUnitWatcher;
    };

} // ns
