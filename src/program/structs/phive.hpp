#pragma once

#include <cstddef>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>

#include "lib.hpp"
#include "syms_merged.hpp"


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

        void getAABB(sead::BoundBox3f* dst) {
            using impl_t = void (RigidBodyBase*, sead::BoundBox3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::getAABB::offset));
            return impl(this, dst); // TODO magic: get current func to macro+interpolate?  ^---------------------------^
        }

        void getBoundingBoxWorld(sead::BoundBox3f* dst) {
            using impl_t = void (RigidBodyBase*, sead::BoundBox3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::getBoundingBoxWorld::offset));
            return impl(this, dst);
        }

        const char* getName() {
            using impl_t = const char* (RigidBodyBase*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::getName::offset));
            return impl(this);
        }

        void requestSetLinearVelocity(sead::Vector3f* vel) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::requestSetLinearVelocity::offset));
            return impl(this, vel);
        }

        void requestSetLinearVelocityXYZ(float x, float y, float z) {
            auto vel = sead::Vector3f{x, y, z};
            return this->requestSetLinearVelocity(&vel);
        }

        void applyLinearImpulse(sead::Vector3f* val) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::applyLinearImpulse::offset));
            return impl(this, val);
        }

        void applyLinearImpulseXYZ(float x, float y, float z) {
            auto val = sead::Vector3f{x, y, z};
            return this->applyLinearImpulse(&val);
        }

        void getNextLinearVelocity(sead::Vector3f* dst) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::getNextLinearVelocity::offset));
            return impl(this, dst);
        }

    };
    static_assert(offsetof(RigidBodyBase, lastTransform) == 0x98);

    class RigidBodyEntity: public RigidBodyBase {
        public:
        ::engine::actor::ActorBaseLink* getActorLink() {
            using impl_t = ::engine::actor::ActorBaseLink* (RigidBodyEntity*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyEntity::getActorLink::offset));
            return impl(this); // TODO default &game::actor::sDefaultActorLink?
        }
    };

    /*
    class CharacterMatterRigidBodyEntity: public RigidBodyEntity {
        public:
        // XXX these behave the same as RigidBodyBase::requestSetLinearVelocity on Player, usually no effect
        void requestSetLinearVelocity(sead::Vector3f* vel) {
            using impl_t = void (RigidBodyBase*, sead::Vector3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::CharacterMatterRigidBodyEntity::requestSetLinearVelocity::offset));
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
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::ControllerSet::visitRigidBodyEntities::offset));
            auto cb_ = &cb; // just accept a normal function pointer, this was nuts
            return impl(this, &cb_);
        }
    };

} // ns

