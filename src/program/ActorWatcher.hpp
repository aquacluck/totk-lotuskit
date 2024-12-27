#pragma once
#include <cstring>
#include <gfx/seadPrimitiveRenderer.h>
#include "structs/engineActor.hpp"
#include "structs/phive.hpp"
#include "TextWriter.hpp"
#include "PrimitiveDrawer.hpp"

namespace lotuskit {
    class ActorWatcherEntry {
        public:
        // state (freely cleared+mutated by the mod)
        bool isCalc; // enable this slot to run
        bool isPendingSelection;
        u8 selectionType; // 0 = unused, 1 = next spawn (TODO?), 2 = next relation to parent_i (TODO?), 3 = preactor, 4 = recall hover
        engine::actor::ActorBase* actorSelectorParentRelation;
        engine::actor::ActorBase* actor;
        engine::actor::PreActor* preactor;
        //engine::actor::CreateWatcherRef createWatcher; // using this in requestCreateActorAsync gets populated with CreateWatcher::status=2, but actor doesnt spawn... something else to set?
        // TODO Player hknpMotion?

        // config (usually only changed on request)
        bool doTextWriter;
        bool doWsLog; // (cannot suppress slot assign/clear messages, just target actor data)

        bool doDrawAABB; // yellow wirecube
        bool doDrawPos; // yellow dot
        bool doDrawVel; // blue bars
        bool doDrawAngVel; // blue disks
                           //
        bool doDrawModelPos; // gray dot

        u64 doDrawRigidBodyAABB; // green wirecube
        u64 doDrawRigidBodyPos; // green dot
        u64 doDrawRigidBodyPosPast; // faint green
        u64 doDrawRigidBodyPosFuture; // bold green

        ActorWatcherEntry() {
            this->clear(); // state

            // config // TODO load defaults from json? generic kv settings api for ws?
            this->doTextWriter = true;
            this->doWsLog = true;

            this->doDrawAABB = false;
            this->doDrawPos = false;
            this->doDrawVel = false;
            this->doDrawAngVel = false;

            this->doDrawModelPos = false;

            this->doDrawRigidBodyAABB = 0;
            this->doDrawRigidBodyPos = 0;
            this->doDrawRigidBodyPosPast = 0;
            this->doDrawRigidBodyPosFuture = 0;
        }

        void clear() {
            // state
            this->isCalc = false;
            this->isPendingSelection = false;
            this->selectionType = 0;
            this->actorSelectorParentRelation = nullptr;
            this->actor = nullptr;
            this->preactor = nullptr;
            //this->createWatcher.watcher = nullptr;
            //this->trackedMotion = nullptr;
        }

        void wsAnnounceConfig(size_t this_i) {
            char ns[16] = "/ActorWatcher/i";
            ns[14] = '0' + this_i; // replace i
            Logger::logJson(json::object({
                {"doTextWriter", this->doTextWriter},
                {"doWsLog", this->doWsLog},

                {"doDrawAABB", this->doDrawAABB},
                {"doDrawPos", this->doDrawPos},
                {"doDrawVel", this->doDrawVel},
                {"doDrawAngVel", this->doDrawAngVel},

                {"doDrawModelPos", this->doDrawModelPos},

                {"doDrawRigidBodyAABB", this->doDrawRigidBodyAABB},
                {"doDrawRigidBodyPos", this->doDrawRigidBodyPos},
                {"doDrawRigidBodyPosPast", this->doDrawRigidBodyPosPast},
                {"doDrawRigidBodyPosFuture", this->doDrawRigidBodyPosFuture}

            }), ns, false, false); // noblock, no debug log
        }

        void wsAnnounceState(size_t this_i) {
            // most state is published in calc(), but we might want to report eg pending selection details
            char ns[16] = "/ActorWatcher/i";
            ns[14] = '0' + this_i; // replace i
            Logger::logJson(json::object({
                {"actorPtr", (u64)this->actor}
            }), ns, false, false); // noblock, no debug log
        }

    };

    class ActorWatcher {
        public:
        static constexpr size_t MAX_WATCHER_SLOTS = 4;
        inline static ActorWatcherEntry slots[MAX_WATCHER_SLOTS] = {};

        inline static void clearSlot(size_t i) { slots[i].clear(); slots[i].wsAnnounceState(i); }
        inline static void pauseSlot(size_t i) { slots[i].isCalc = false; slots[i].wsAnnounceState(i); }
        inline static void assignSlot(size_t i, engine::actor::ActorBase* actor) {
            auto& slot = slots[i];
            slot.clear();
            slot.isCalc = true;
            slot.actor = actor;
            slot.wsAnnounceState(i);
            slot.wsAnnounceConfig(i);
        }
        inline static void assignSlotAwaitRecall(size_t i) {
            auto& slot = slots[i];
            slot.clear();
            slot.isCalc = true;
            slot.selectionType = 4; // recall
            slot.isPendingSelection = true;
            slot.wsAnnounceState(i);
            slot.wsAnnounceConfig(i);
        }
        inline static void assignSlotPreActor(size_t i, engine::actor::PreActor* preactor) {
            auto& slot = slots[i];
            slot.clear();
            slot.isCalc = true;
            slot.preactor = preactor;
            slot.selectionType = 3; // preactor
            slot.isPendingSelection = true;
            slot.wsAnnounceState(i);
            slot.wsAnnounceConfig(i);
        }
        inline static engine::actor::ActorBase* getSlotActor(size_t i) {
            // FIXME don't return/draw destroyed actors
            return slots[i].actor;
        }

        // config
        inline static void doTextWriter(size_t i, bool val) {
            slots[i].doTextWriter = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doWsLog(size_t i, bool val) {
            slots[i].doWsLog = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawAABB(size_t i, bool val) {
            slots[i].doDrawAABB = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawPos(size_t i, bool val) {
            slots[i].doDrawPos = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawVel(size_t i, bool val) {
            slots[i].doDrawVel = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawAngVel(size_t i, bool val) {
            slots[i].doDrawAngVel = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawModelPos(size_t i, bool val) {
            slots[i].doDrawModelPos = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawRigidBodyAABB(size_t i, u64 val) {
            slots[i].doDrawRigidBodyAABB = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawRigidBodyPos(size_t i, u64 val) {
            slots[i].doDrawRigidBodyPos = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawRigidBodyPosPast(size_t i, u64 val) {
            slots[i].doDrawRigidBodyPosPast = val;
            slots[i].wsAnnounceConfig(i);
        }
        inline static void doDrawRigidBodyPosFuture(size_t i, u64 val) {
            slots[i].doDrawRigidBodyPosFuture = val;
            slots[i].wsAnnounceConfig(i);
        }

        inline static void resolveRecallHighlight(engine::actor::ActorBase *actor) {
            //if (actor == nullptr) { return; }
            for (u8 i=0; i < MAX_WATCHER_SLOTS; i++) {
                auto& slot = slots[i];
                if (!slot.isCalc) { continue; }
                if (slot.isPendingSelection && slot.selectionType == 4) {
                    ActorWatcher::assignSlot(i, actor);
                    // if multiple slots are watching recall, next frame will fill next slot ig?
                    return;
                }
            }
        }

        inline static void calc() {
            for (u8 i=0; i < MAX_WATCHER_SLOTS; i++) {
                auto& slot = slots[i];
                if (!slot.isCalc) { continue; } // disabled

                // ActorWatchers may select their target actor with:
                // - polling something each frame (below)
                // - hook code covering specific cases (eg Recall)
                // - scripts calling things like actor::createAndWatch or ActorWatcher::assignSlot

                if (slot.isPendingSelection && slot.preactor != nullptr && slot.selectionType == 3) {
                    // Resolve pending actor selection via preactors
                    if (slot.preactor->mActor != nullptr) {
                        slot.actor = slot.preactor->mActor;
                        slot.isPendingSelection = false;
                        slot.wsAnnounceState(i);
                    } else if (slot.doTextWriter) {
                        lotuskit::TextWriter::printf(
                            0, "ActorWatcher[%d] awaiting PreActor(%p) \n\n",
                            i, slot.preactor
                        );
                    }
                }
                if (slot.isPendingSelection && slot.selectionType == 4 && slot.doTextWriter) {
                    lotuskit::TextWriter::printf(0, "ActorWatcher[%d] awaiting Recall \n\n", i);
                }


                // log selected actor
                if (slot.actor == nullptr) { continue; } // no actor found
                const auto actor = slot.actor;
                const auto rot = actor->mRotation;

                if (slot.doWsLog) {
                    char ns[16] = "/ActorWatcher/i";
                    ns[14] = '0' + i; // replace i
                    Logger::logJson(json::object({
                        {"actorName", actor->mName.cstr()},
                        {"actorPtr", (u64)slot.actor},
                        {"pos", json::array({ actor->mPosition.x, actor->mPosition.y, actor->mPosition.z })},
                        {"vel", json::array({ actor->mLastLinearVelocity.x, actor->mLastLinearVelocity.y, actor->mLastLinearVelocity.z })},
                        {"rot", json::array({ rot.m[0][0], rot.m[0][1], rot.m[0][2], rot.m[1][0], rot.m[1][1], rot.m[1][2], rot.m[2][0], rot.m[2][1], rot.m[2][2] })}
                    }), ns, false, false); // noblock, no debug log
                }

                if (slot.doTextWriter) {
                    lotuskit::TextWriter::printf(
                        0, "%s(%p) \npos: %f, %f, %f \nrot: [%f, %f, %f, %f, %f, %f, %f, %f, %f] \nvel: %f, %f, %f \nangvel: %f %f %f \n\n",
                        actor->mName.cstr(), slot.actor,
                        actor->mPosition.x, actor->mPosition.y, actor->mPosition.z,
                        rot.m[0][0], rot.m[0][1], rot.m[0][2], rot.m[1][0], rot.m[1][1], rot.m[1][2], rot.m[2][0], rot.m[2][1], rot.m[2][2],
                        actor->mLastLinearVelocity.x, actor->mLastLinearVelocity.y, actor->mLastLinearVelocity.z,
                        actor->mLastAngularVelocity.x, actor->mLastAngularVelocity.y, actor->mLastAngularVelocity.z
                    );
                }

                // PrimitiveDrawer info

                // TODO optional base color per actorwatcher for busy scenes?
                //const sead::Color4f Transparent{0,0,0,0};
                //const sead::Color4f White{1,1,1,1};
                const sead::Color4f VoidGray = sead::Color4f(0.3, 0.3, 0.3, 0.15);
                const sead::Color4f VoidGrayPast = sead::Color4f(0.5, 0.5, 0.5, 0.1); // faint
                //const sead::Color4f VoidGrayFuture = sead::Color4f(0.1, 0.1, 0.1, 0.2); // darker+heavier

                const sead::Color4f RecallYellow{0.9f, 0.76f, 0.16f, 0.15f}; // for position+rotation
                const sead::Color4f MotionBlue{0.16f, 0.3f, 0.9f, 0.3f};
                const sead::Color4f PhysicalGreen = sead::Color4f(0.4, 0.9, 0.4, 0.15);
                const sead::Color4f PhysicalGreenPast = sead::Color4f(0.4, 0.9, 0.4, 0.1); // faint
                const sead::Color4f PhysicalGreenFuture = sead::Color4f(0.2, 1.0, 0.2, 0.2); // greener+heavier

                sead::Matrix34f mtx = actor->getTransform();

                if (slot.doDrawAABB || slot.doDrawPos) {
                    lotuskit::PrimitiveDrawer::setModelMtx(0, mtx);
                }
                if (slot.doDrawPos) {
                    const float radius = 0.1; // TODO adjust radius * camera distance to ensure min size on screen?
                    lotuskit::PrimitiveDrawer::drawSphere4x8(0, sead::Vector3f(0, 0, 0), radius, RecallYellow, RecallYellow);
                }
                if (slot.doDrawAABB) {
                    auto* aabb = &(actor->mAABB);
                    lotuskit::PrimitiveDrawer::drawWireCube(0, sead::PrimitiveDrawer::CubeArg(*aabb, RecallYellow));
                }
                if (slot.doDrawVel) {
                    const float radius = 0.05;
                    sead::Vector3f vel = actor->mLastLinearVelocity;
                    const float mag = vel.length();
                    if (std::fabsf(mag) > 0.01) {
                        sead::Matrix33f vrot;

                        float vx = std::log2f(std::fabsf(vel.x)+2.0f);
                        if (vx > 0) {
                            vx = (vel.x < 0 ? -1 : 1) * ((vx >= 1.0f) ? (vx - 1.0f) : vx);
                            vrot = {0,1,0, 0,0,-1, -1,0,0}; // facing down, up is x+ east
                            lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, actor->mPosition));
                            lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0, vx * 0.5f, 0), radius, vx, MotionBlue, MotionBlue);
                        }

                        float vy = std::log2f(std::fabsf(vel.y)+2.0f);
                        if (vy > 0) {
                            vy = (vel.y < 0 ? -1 : 1) * ((vy >= 1.0f) ? (vy - 1.0f) : vy);
                            vrot = {1,0,0, 0,1,0, 0,0,1}; // (identity) facing level south: up is up
                            lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, actor->mPosition));
                            lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0, vy * 0.5f, 0), radius, vy, MotionBlue, MotionBlue);
                        }

                        float vz = std::log2f(std::fabsf(vel.z)+2.0f);
                        if (vz > 0) {
                            vz = (vel.z < 0 ? -1 : 1) * ((vz >= 1.0f) ? (vz - 1.0f) : vz);
                            vrot = {1,0,0, 0,0,-1, 0,1,0}; // facing down, up is z+ south
                            lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, actor->mPosition));
                            lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0, vz * 0.5f, 0), radius, vz, MotionBlue, MotionBlue);
                        }
                    }
                }

                if (slot.doDrawAngVel) {
                    // similar calc as linear velocity, but no signedness, just a "spin speed" disk radius
                    sead::Vector3f vel = actor->mLastAngularVelocity;
                    const float mag = vel.length();
                    if (std::fabsf(mag) > 0.01) {
                        sead::Matrix33f vrot;

                        float vx = std::log2f(std::fabsf(vel.x)+2.0f);
                        if (vx > 0) {
                            vx = ((vx >= 1.0f) ? (vx - 1.0f) : vx);
                            vrot = {0,0,1, 0,1,0, -1,0,0}; // facing level east, up is up
                            lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, actor->mPosition));
                            lotuskit::PrimitiveDrawer::drawDisk32(0, sead::Vector3f(0,0,0), vx*0.5f, MotionBlue, MotionBlue);
                        }

                        float vy = std::log2f(std::fabsf(vel.y)+2.0f);
                        if (vy > 0) {
                            vy = ((vy >= 1.0f) ? (vy - 1.0f) : vy);
                            vrot = {0,1,0, 0,0,-1, -1,0,0}; // facing down, up is x+ east
                            lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, actor->mPosition));
                            lotuskit::PrimitiveDrawer::drawDisk32(0, sead::Vector3f(0,0,0), vy*0.5f, MotionBlue, MotionBlue);
                        }

                        float vz = std::log2f(std::fabsf(vel.z)+2.0f);
                        if (vz > 0) {
                            vz = ((vz >= 1.0f) ? (vz - 1.0f) : vz);
                            vrot = {1,0,0, 0,1,0, 0,0,1}; // (identity) facing level south: up is up
                            lotuskit::PrimitiveDrawer::setModelMtx(0, sead::Matrix34f(vrot, actor->mPosition));
                            lotuskit::PrimitiveDrawer::drawDisk32(0, sead::Vector3f(0,0,0), vz*0.5f, MotionBlue, MotionBlue);
                        }
                    }

                    //sead::Color4f color2{0.45f, 0.1f, 0.87f, 0.1f};
                    //lotuskit::PrimitiveDrawer::drawSphere8x16(0, sead::Vector3f(0.f, 0.f, 0.f), radius, color1, color2);
                    //lotuskit::PrimitiveDrawer::drawDisk32(0, sead::Vector3f(0.f, 0.f, 0.f), 2.f, color1, color2);
                    //lotuskit::PrimitiveDrawer::drawCylinder32(0, sead::Vector3f(0.f, height * 0.5f, 0.f), radius, height, color1, color1);
                }

                auto physCmp = actor->getPhysicsComponent();
                if (physCmp && physCmp->controllerSet && physCmp->controllerSet->mainRigidBody) {

                    // TODO multiple rigidbodies bitflagged? try chests, springs, pots, wheels?
                    // TODO extract rigidbody logging/drawing for use outside actors, draw global(?), ...
                    const auto rbody = physCmp->controllerSet->mainRigidBody;

                    if (false) {
                        // TODO ws log rigidbodies for frontend?
                        const char* name = rbody->getName();

                        sead::BoundBox3f aabb;
                        sead::BoundBox3f wrld;
                        rbody->getAABB(&aabb);
                        rbody->getBoundingBoxWorld(&wrld);

                        lotuskit::TextWriter::printf(
                            0, "RigidBody %s(%p) \naabb : [%f, %f] [%f, %f] [%f, %f] \nworld: [%f, %f] [%f, %f] [%f, %f] \n\n",
                            name, rbody,
                            aabb.getMin().x, aabb.getMax().x, aabb.getMin().y, aabb.getMax().y, aabb.getMin().z, aabb.getMax().z,
                            wrld.getMin().x, wrld.getMax().x, wrld.getMin().y, wrld.getMax().y, wrld.getMin().z, wrld.getMax().z
                        );
                    }

                    // any flag = all are turned on. TODO bitflag decider impl
                    const bool doDrawRigidBodyAABB = slot.doDrawRigidBodyAABB > 0;
                    const bool doDrawRigidBodyPos = slot.doDrawRigidBodyPos > 0;
                    const bool doDrawRigidBodyPosPast = slot.doDrawRigidBodyPosPast > 0;
                    const bool doDrawRigidBodyPosFuture = slot.doDrawRigidBodyPosFuture > 0;

                    if (doDrawRigidBodyPos || doDrawRigidBodyAABB) {
                        lotuskit::PrimitiveDrawer::setModelMtx(0, rbody->lastTransform);
                    }
                    if (doDrawRigidBodyPos) {
                        lotuskit::PrimitiveDrawer::drawSphere8x16(0, sead::Vector3f(0,0,0), 0.1, PhysicalGreen, PhysicalGreen);
                    }
                    if (doDrawRigidBodyAABB) {
                        sead::BoundBox3f aabb;
                        rbody->getAABB(&aabb);
                        lotuskit::PrimitiveDrawer::drawWireCube(0, sead::PrimitiveDrawer::CubeArg(aabb, PhysicalGreen));
                    }

                    if (doDrawRigidBodyPosPast) {
                        lotuskit::PrimitiveDrawer::setModelMtx(0, rbody->prevTransform);
                        lotuskit::PrimitiveDrawer::drawSphere8x16(0, sead::Vector3f(0,0,0), 0.075, PhysicalGreenPast, VoidGrayPast);
                    }

                    // sometimes a "next" is available
                    const bool isChangeTransform = rbody->changeRequest && (rbody->changeRequest->flags >> 6 & 1) != 0;
                    if (doDrawRigidBodyPosFuture && isChangeTransform) {
                        //sead::Matrix34f current = isChangeTransform ? rbody->changeRequest->nextTransform : rbody->lastTransform;
                        lotuskit::PrimitiveDrawer::setModelMtx(0, rbody->changeRequest->nextTransform);
                        lotuskit::PrimitiveDrawer::drawSphere8x16(0, sead::Vector3f(0,0,0), 0.075, PhysicalGreenFuture, PhysicalGreenFuture);
                    }

                } // main rigidbody

                if (slot.doDrawModelPos) {
                    auto modelCmp = actor->getModelComponent();
                    if (modelCmp && modelCmp->model) {
                        lotuskit::PrimitiveDrawer::setModelMtx(0, modelCmp->model->modelMtx);
                        lotuskit::PrimitiveDrawer::drawSphere8x16(0, sead::Vector3f(0,0,0), 0.1, VoidGray, VoidGray);

                    } // do draw
                } // allow draw

            } // foreach slot
        } // calc
    }; // ActorWatcher
} // ns
