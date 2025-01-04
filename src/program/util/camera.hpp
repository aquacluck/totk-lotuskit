#pragma once
#include <lib.hpp>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <string>

namespace lotuskit::util::camera {

    // ty zeldar
    struct DisgustingLookAtCamera {
        char _0[0x50];
        sead::Vector3f mUp;
        int _5C;
        sead::Vector3<double> mPos;
        sead::Vector3<double> mAt;
    };

    inline bool doFreeze = false;
    inline sead::Vector3f frozenUp = {0,0,0};
    inline sead::Vector3<double> frozenPos = {0,0,0};
    inline sead::Vector3<double> frozenAt = {0,0,0};

    inline void toggleFreeze() { doFreeze = !doFreeze; }

    inline void freeze333(float u0, float u1, float u2, double p0, double p1, double p2, double a0, double a1, double a2) {
        doFreeze = true; // FIXME AS vector bindings
        frozenUp = {u0, u1, u2};
        frozenPos = {p0, p1, p2};
        frozenAt = {a0, a1, a2};
    }

    inline void log() {
        Logger::logJson(json::object({
            {"up", json::array({ frozenUp.x, frozenUp.y, frozenUp.z })},
            {"pos", json::array({ frozenPos.x, frozenPos.y, frozenPos.z })},
            {"at", json::array({ frozenAt.x, frozenAt.y, frozenAt.z })}
        }), "/camera/log", false, false); // noblock, no debug log
    }

    inline void disgustingCameraHook(DisgustingLookAtCamera* thisPtr, sead::Matrix34f* dst) {
        if (doFreeze) {
            // affects all cameras
            thisPtr->mUp = frozenUp;
            thisPtr->mPos = {frozenPos.x, frozenPos.y, frozenPos.z};
            thisPtr->mAt = {frozenAt.x, frozenAt.y, frozenAt.z};
        } else {
            frozenUp = thisPtr->mUp;
            frozenPos = {thisPtr->mPos.x, thisPtr->mPos.y, thisPtr->mPos.z};
            frozenAt = {thisPtr->mAt.x, thisPtr->mAt.y, thisPtr->mAt.z};
        }

        using impl_t = void (DisgustingLookAtCamera*, sead::Matrix34f*);
        auto impl = reinterpret_cast<impl_t*>(exl::util::modules::GetTargetOffset(sym::sead::LookAtCamera::doUpdateMatrix::offset));
        return impl(thisPtr, dst);
    }

} // ns

