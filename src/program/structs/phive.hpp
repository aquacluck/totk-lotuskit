#pragma once

#include <cstddef>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>
#include "exlaunch.hpp"
#include "util/alloc.hpp"
using String = lotuskit::String;

namespace engine::actor { class ActorBaseLink; }

namespace phive {

    class ChangeRequest { // XXX RigidBodyChangeRequest?
        public:
        char _00[0x8];
        sead::Matrix34f nextTransform;
        char _38[0x9c];
        u32 flags;
    };

    class RigidBodyBase {
        public:
        char _00[0x60];
        ChangeRequest* changeRequest;
        char _68[0x30];
        sead::Matrix34f lastTransform;
        sead::Matrix34f prevTransform;
        // XXX pad more members before RigidBodyEntity?

        void getAABB_(sead::BoundBox3f* dst) {
            using impl_t = void (RigidBodyBase*, sead::BoundBox3f*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyBase::getAABB");
            return impl(this, dst); // TODO magic: get current func string to ^ pass?
        }

        sead::BoundBox3f getAABB() {
            sead::BoundBox3f out = {0,0,0,0,0,0};
            this->getAABB_(&out);
            return out;
        }

        void getBoundingBoxWorld_(sead::BoundBox3f* dst) {
            using impl_t = void (RigidBodyBase*, sead::BoundBox3f*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyBase::getBoundingBoxWorld");
            return impl(this, dst);
        }

        sead::BoundBox3f getBoundingBoxWorld() {
            sead::BoundBox3f out = {0,0,0,0,0,0};
            this->getBoundingBoxWorld_(&out);
            return out;
        }

        String getName() {
            using impl_t = const char* (RigidBodyBase*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyBase::getName");
            return impl(this);
        }

        void requestSetLinearVelocity(sead::Vector3f* vel) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyBase::requestSetLinearVelocity");
            return impl(this, vel);
        }

        void requestSetLinearVelocityXYZ(float x, float y, float z) {
            auto vel = sead::Vector3f{x, y, z};
            return this->requestSetLinearVelocity(&vel);
        }

        void applyLinearImpulse(sead::Vector3f* val) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyBase::applyLinearImpulse");
            return impl(this, val);
        }

        void applyLinearImpulseXYZ(float x, float y, float z) {
            auto val = sead::Vector3f{x, y, z};
            return this->applyLinearImpulse(&val);
        }

        void getNextLinearVelocity_(sead::Vector3f* dst) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyBase::getNextLinearVelocity");
            return impl(this, dst);
        }

        sead::Vector3f getNextLinearVelocity() {
            sead::Vector3f out = {0,0,0};
            this->getNextLinearVelocity_(&out);
            return out;
        }

    };
    static_assert(offsetof(RigidBodyBase, lastTransform) == 0x98);

    class RigidBodyEntity: public RigidBodyBase {
        public:
        ::engine::actor::ActorBaseLink* getActorLink() {
            using impl_t = ::engine::actor::ActorBaseLink* (RigidBodyEntity*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::RigidBodyEntity::getActorLink");
            return impl(this); // TODO default &game::actor::sDefaultActorLink?
        }
    };

    /*
    class CharacterMatterRigidBodyEntity: public RigidBodyEntity {
        public:
        // XXX these behave the same as RigidBodyBase::requestSetLinearVelocity on Player, usually no effect
        void requestSetLinearVelocity(sead::Vector3f* vel) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::CharacterMatterRigidBodyEntity::requestSetLinearVelocity");
            return impl(this, vel);
        }

        void requestSetLinearVelocityXYZ(float x, float y, float z) {
            auto vel = sead::Vector3f{x, y, z};
            return this->requestSetLinearVelocity(&vel);
        }
    };
    */

    using ControllerSet_visitRigidBodyEntities_cb = void(*)(void*, RigidBodyEntity*, char*);

    class ControllerSet {
        public:
        char _00[0x150];
        RigidBodyEntity* mainRigidBody;

        void visitRigidBodyEntities(ControllerSet_visitRigidBodyEntities_cb cb) {
            using impl_t = void (ControllerSet*, ControllerSet_visitRigidBodyEntities_cb**);
            auto impl = EXL_SYM_RESOLVE<impl_t*>("phive::ControllerSet::visitRigidBodyEntities");
            auto cb_ = &cb; // just accept a normal function pointer, this was nuts
            return impl(this, &cb_);
        }
    };

} // ns

