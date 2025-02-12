#pragma once
#include "exlaunch.hpp"
#include "Logger.hpp"
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <string>

namespace lotuskit::util::camera {

    // ty zeldar
    struct DisgustingLookAtCamera {
        void* mVtable;
        sead::Matrix34f mCameraMtx;
        sead::Vector3f mPos32;
        sead::Vector3f mAt32;
        sead::Vector3f mUp32;
        u32 _pad;
        sead::Vector3<double> mPos64;
        sead::Vector3<double> mAt64;
    };

    inline bool doFreeze = false;
    inline void toggleFreeze() { doFreeze = !doFreeze; }

    // freeze data
    inline sead::Vector3<double> frozenPos = {0,0,0};
    inline sead::Vector3<double> frozenAt = {0,0,0};
    inline sead::Vector3f frozenUp = {0,1,0};

    // ActorWatcher follow data
    inline bool doFollowActor = false;
    inline bool doFollowActorRelativeRotate = false;
    inline u64 followSlot = 0;
    inline float followDist = 20;
    inline float followElev = 0; // degrees vertical
    inline sead::Vector3f followDir = {0,0,0};
    void freezeAtActorWatcher(u64 slot_i, float dist, const sead::Vector3f &dir, float elev, bool follow, bool relative);
    void followActorImpl(); // updates freeze data

    inline void freeze333(float u0, float u1, float u2, double p0, double p1, double p2, double a0, double a1, double a2) {
        frozenPos = {p0, p1, p2};
        frozenAt = {a0, a1, a2};
        frozenUp = {u0, u1, u2};
        doFreeze = true;
        doFollowActor = false;
    }

    inline void log() {
        Logger::logJson(json::object({
            {"pos", json::array({ frozenPos.x, frozenPos.y, frozenPos.z })},
            {"at", json::array({ frozenAt.x, frozenAt.y, frozenAt.z })},
            {"up", json::array({ frozenUp.x, frozenUp.y, frozenUp.z })}
        }), "/camera/log", false, false); // noblock, no debug log
    }

    inline void disgustingCameraHook(DisgustingLookAtCamera* self, sead::Matrix34f* dst) {
        if (doFreeze) {
            if (doFollowActor) { followActorImpl(); }
            // affects all cameras
            self->mPos32 = {(float)frozenPos.x, (float)frozenPos.y, (float)frozenPos.z};
            self->mPos64 = {frozenPos.x, frozenPos.y, frozenPos.z};
            self->mAt32 = {(float)frozenAt.x, (float)frozenAt.y, (float)frozenAt.z};
            self->mAt64 = {frozenAt.x, frozenAt.y, frozenAt.z};
            self->mUp32 = frozenUp;
        } else {
            // record camera
            frozenPos = {self->mPos64.x, self->mPos64.y, self->mPos64.z};
            frozenAt = {self->mAt64.x, self->mAt64.y, self->mAt64.z};
            frozenUp = self->mUp32;
        }

        using impl_t = void (DisgustingLookAtCamera*, sead::Matrix34f*);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("sead::LookAtCamera::doUpdateMatrix");
        return impl(self, dst);
    }

} // ns

