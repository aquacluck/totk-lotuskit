#pragma once
#include <cstring>
#include "structs/engineActor.hpp"
#include "TextWriter.hpp"

namespace lotuskit {
    class ActorWatcherEntry {
        public:
        bool isCalc; // observe actor, to eg TODO watch/search and report to the frontend without drawing
        bool isDraw; // textwriter output
        bool isPendingSelection;
        u8 selectionType; // 0 = unused, 1 = next spawn, 2 = next relation to parent_i
        engine::actor::ActorBase* actorSelectorParentRelation;
        engine::actor::ActorBase* actor;
        // TODO preactor
        // TODO Player hknpMotion?

        void clear() {
            this->isCalc = false;
            this->isDraw = false;
            this->isPendingSelection = false;
            this->selectionType = 0;
            this->actorSelectorParentRelation = nullptr;
            this->actor = nullptr;
            //this->preactor = nullptr;
            //this->trackedMotion = nullptr;
        }
    };

    class ActorWatcher {
        public:
        static constexpr size_t MAX_WATCHER_SLOTS = 4;
        inline static ActorWatcherEntry slots[MAX_WATCHER_SLOTS] = {0};

        inline static void clearSlot(size_t i) { slots[i].clear(); }
        inline static void pauseSlot(size_t i) { slots[i].isCalc = false; }
        inline static void assignSlot(size_t i, engine::actor::ActorBase* actor) {
            auto& slot = slots[i];
            slot.clear();
            slot.isCalc = true;
            slot.isDraw = true;
            slot.actor = actor;
        }

        inline static void calc() {
            for (u8 i=0; i < MAX_WATCHER_SLOTS; i++) {
                auto& slot = slots[i];

                if (slot.isCalc && slot.actor != nullptr) {
                    // TODO Logger::logJson(actorstuff)
                }

                if (slot.isDraw && slot.actor != nullptr) {
                    const auto rot = slot.actor->mRotation;
                    lotuskit::TextWriter::printf(
                        0, "%s(%p) \npos: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f] \n\n",
                        slot.actor->mName.cstr(), slot.actor,
                        slot.actor->mPosition.x, slot.actor->mPosition.y, slot.actor->mPosition.z,
                        rot.m[0][0], rot.m[0][1], rot.m[0][2], rot.m[1][0], rot.m[1][1], rot.m[1][2], rot.m[2][0], rot.m[2][1], rot.m[2][2]
                    );
                }
            }
        }

    };
}
