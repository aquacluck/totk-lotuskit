#pragma once

#include <cstddef>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <math/seadMatrix.h>

namespace phive {

    class ChangeRequest { // XXX RigidBodyChangeRequest?
        public:
        char _00[0x8];
        sead::Matrix34f nextTransform;
        char _38[0x9c];
        u32 flags;
    };

    class RigidBody { // XXX RigidBodyBase? RigidBodyEntity?
        public:
        char _00[0x60];
        ChangeRequest* changeRequest;
        char _68[0x30];
        sead::Matrix34f lastTransform;
        sead::Matrix34f prevTransform;
    };
    static_assert(offsetof(RigidBody, lastTransform) == 0x98);

    class ControllerSet { // XXX RigidBodyController?
        public:
        char _00[0x150];
        RigidBody* mainRigidBody;
    };

} // ns

