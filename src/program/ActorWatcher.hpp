#pragma once
#include <cstring>
#include <gfx/seadPrimitiveRenderer.h>
#include "structs/engineActor.hpp"
#include "TextWriter.hpp"
#include "PrimitiveDrawer.hpp"

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
        bool doDrawAABB; // yellow wirecube
        bool doDrawPos; // yellow dot
        bool doDrawVel; // blue bars

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
            this->doDrawAABB = false; // TODO load defaults from json? generic kv settings api for ws?
            this->doDrawPos = true;
            this->doDrawVel = true;
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
        inline static void doDrawAABB(size_t i, bool val) { slots[i].doDrawAABB = val; }
        inline static void doDrawPos(size_t i, bool val) { slots[i].doDrawPos = val; }
        inline static void doDrawVel(size_t i, bool val) { slots[i].doDrawVel = val; }

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

                    // PrimitiveDrawer info
                    // TODO optional base color per actorwatcher for busy scenes?
                    const sead::Color4f RecallYellow{0.9f, 0.76f, 0.16f, 0.15f}; // for position+rotation
                    sead::Matrix34f mtx = slot.actor->getTransform();

                    if (slot.doDrawAABB || slot.doDrawPos) {
                        lotuskit::PrimitiveDrawer::setModelMtx(0, mtx);
                    }
                    if (slot.doDrawPos) {
                        const float radius = 0.1; // TODO adjust radius * camera distance to ensure min size on screen?
                        lotuskit::PrimitiveDrawer::drawSphere4x8(0, sead::Vector3f(0, 0, 0), radius, RecallYellow, RecallYellow);
                    }
                    if (slot.doDrawAABB) {
                        auto* aabb = &(slot.actor->mAABB);
                        lotuskit::PrimitiveDrawer::drawWireCube(0, sead::PrimitiveDrawer::CubeArg(*aabb, RecallYellow));
                    }
                    if (slot.doDrawVel) {
                        const sead::Color4f MotionBlue{0.16f, 0.3f, 0.9f, 0.5f};
                        const float radius = 0.05;
                        sead::Vector3f vel = slot.actor->mLastLinearVelocity;
                        const float mag = vel.length();
                        if (std::fabsf(mag) > 0.01) {
                            sead::Matrix33f vrot;

                            float vx = std::log2f(std::fabsf(vel.x)+2.0f);
                            if (vx > 0) {
                                vx = (vel.x < 0 ? -1 : 1) * ((vx >= 1.0f) ? (vx - 1.0f) : vx);
                                vrot = {0,1,0, 0,0,-1, -1,0,0}; // facing down, up is x+ east
                                lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, slot.actor->mPosition));
                                lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0, vx * 0.5f, 0), radius, vx, MotionBlue, MotionBlue);
                            }

                            float vy = std::log2f(std::fabsf(vel.y)+2.0f);
                            if (vy > 0) {
                                vy = (vel.y < 0 ? -1 : 1) * ((vy >= 1.0f) ? (vy - 1.0f) : vy);
                                vrot = {1,0,0, 0,1,0, 0,0,1}; // (identity) facing level south: up is up
                                lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, slot.actor->mPosition));
                                lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0, vy * 0.5f, 0), radius, vy, MotionBlue, MotionBlue);
                            }

                            float vz = std::log2f(std::fabsf(vel.z)+2.0f);
                            if (vz > 0) {
                                vz = (vel.z < 0 ? -1 : 1) * ((vz >= 1.0f) ? (vz - 1.0f) : vz);
                                vrot = {1,0,0, 0,0,-1, 0,1,0}; // facing down, up is z+ south
                                lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, slot.actor->mPosition));
                                lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0, vz * 0.5f, 0), radius, vz, MotionBlue, MotionBlue);
                            }
                        }
                    }

                    //sead::Color4f color2{0.45f, 0.1f, 0.87f, 0.1f};
                    //lotuskit::PrimitiveDrawer::drawSphere8x16(0, sead::Vector3f(0.f, 0.f, 0.f), radius, color1, color2);
                    //lotuskit::PrimitiveDrawer::drawDisk32(0, sead::Vector3f(0.f, 0.f, 0.f), 2.f, color1, color2);
                    //lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0.f, height * 0.5f, 0.f), radius, height, color1, color1);
                }
            }
        }

    };
} // ns
