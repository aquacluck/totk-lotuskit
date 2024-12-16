#pragma once
#include "lib.hpp"
#include <string>

namespace lotuskit::util::actor {
    void createSimple(const std::string &actorName);
    void createSimpleXYZ(const std::string &actorName, float x, float y, float z);
    void createAndWatch(u8 slotIndex, const std::string &actorName);
    void createAndWatchXYZ(u8 slotIndex, const std::string &actorName, float x, float y, float z);

} // ns
