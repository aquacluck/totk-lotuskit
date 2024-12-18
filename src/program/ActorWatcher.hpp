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
        u8 selectionType; // 0 = unused, 1 = next spawn, 2 = next relation to parent_i, 3 = preactor
        engine::actor::ActorBase* actorSelectorParentRelation;
        engine::actor::ActorBase* actor;
        engine::actor::PreActor* preactor;
        //engine::actor::CreateWatcherRef createWatcher; // using this in requestCreateActorAsync gets populated with CreateWatcher::status=2, but actor doesnt spawn... something else to set?
        // TODO Player hknpMotion?

        void clear() {
            this->isCalc = false;
            this->isDraw = false;
            this->isPendingSelection = false;
            this->selectionType = 0;
            this->actorSelectorParentRelation = nullptr;
            this->actor = nullptr;
            this->preactor = nullptr;
            //this->createWatcher.watcher = nullptr;
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
        inline static void assignSlotPreActor(size_t i, engine::actor::PreActor* preactor) {
            auto& slot = slots[i];
            slot.clear();
            slot.isCalc = true;
            slot.isDraw = true;
            slot.preactor = preactor;
            slot.selectionType = 3; // preactor
            slot.isPendingSelection = true;
        }
        inline static engine::actor::ActorBase* getSlotActor(size_t i) {
            // FIXME don't return/draw destroyed actors
            return slots[i].actor;
        }

        inline static void calc() {
            for (u8 i=0; i < MAX_WATCHER_SLOTS; i++) {
                auto& slot = slots[i];

                if (slot.isCalc && slot.isPendingSelection && slot.preactor != nullptr && slot.selectionType == 3) {
                    // Resolve pending actor selection via preactors
                    if (slot.preactor->mActor != nullptr) {
                        slot.actor = slot.preactor->mActor;
                        slot.isPendingSelection = false;
                    } else {
                        lotuskit::TextWriter::printf(
                            0, "ActorWatcher[%d] awaiting PreActor(%p) \n\n",
                            i, slot.preactor
                        );
                    }
                }

                if (slot.isCalc && slot.actor != nullptr) {
                    // TODO logging settings
                    char ns[16] = "/ActorWatcher/i";
                    ns[14] = '0' + i; // replace i
                    Logger::logJson(json::object({
                        {"actorName", slot.actor->mName.cstr()},
                        {"actorPtr", (u64)slot.actor},
                        {"pos", json::array({ slot.actor->mPosition.x, slot.actor->mPosition.y, slot.actor->mPosition.z })},
                        {"vel", json::array({ slot.actor->mLastLinearVelocity.x, slot.actor->mLastLinearVelocity.y, slot.actor->mLastLinearVelocity.z })}
                    }), ns, false, false); // noblock, no debug log
                }

                if (slot.isDraw && slot.actor != nullptr) {
                    const auto rot = slot.actor->mRotation;
                    lotuskit::TextWriter::printf(
                        0, "%s(%p) \npos: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f] \nvel: %f, %f, %f \nangvel: %f %f %f \n\n",
                        slot.actor->mName.cstr(), slot.actor,
                        slot.actor->mPosition.x, slot.actor->mPosition.y, slot.actor->mPosition.z,
                        rot.m[0][0], rot.m[0][1], rot.m[0][2], rot.m[1][0], rot.m[1][1], rot.m[1][2], rot.m[2][0], rot.m[2][1], rot.m[2][2],
                        slot.actor->mLastLinearVelocity.x, slot.actor->mLastLinearVelocity.y, slot.actor->mLastLinearVelocity.z,
                        slot.actor->mLastAngularVelocity.x, slot.actor->mLastAngularVelocity.y, slot.actor->mLastAngularVelocity.z
                    );
                }
            }
        }

    };
} // ns
