#pragma once

#include <cstddef>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>

#include "lib.hpp"
#include "syms_merged.hpp"

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

        void getAABB(sead::BoundBox3f* dst) {
            // local coords / size only -- does not contain world coords
            using impl_t = void (RigidBodyBase*, sead::BoundBox3f*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::getAABB::offset));
            return impl(this, dst); // TODO magic: get current func to macro+interpolate?  ^---------------------------^
        }

        const char* getName() {
            using impl_t = const char* (RigidBodyBase*);
            auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::phive::RigidBodyBase::getName::offset));
            return impl(this);
        }
    };
    static_assert(offsetof(RigidBodyBase, lastTransform) == 0x98);

    class ControllerSet { // XXX RigidBodyController?
        public:
        char _00[0x150];
        RigidBodyBase* mainRigidBody;
    };

} // ns

