#pragma once
#include "exlaunch.hpp"
#include <angelscript.h>
#include <nn/hid.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include "structs/nnSixAxis.hpp"

namespace lotuskit::tas {
    struct RecordInput {
        // 24B main input payload
        nn::hid::NpadButtonSet buttons; // u64
        nn::hid::AnalogStickState LStick; // s32 s32
        nn::hid::AnalogStickState RStick; // s32 s32
        // 0x60 gyro, this is not actually recorded/dumped but we can display it, script playback against it etc
        nn::hid::SixAxisSensorState gyro;
    };

    class Record {
        public:
        static void calc(); // when recording: consume VFRMgr->mDeltaFrame and advance to a new tas::input if currentInput has changed

        static RecordInput currentInput; // volatile, overwritten constantly by applyCurrentInput hook
        static RecordInput accumulatorInput; // for keeping track of how long currentInput is held
        static inline u32 accumulatedInput60 = 0;
        static inline bool isRecordActive = false;

        inline static void applyCurrentInput(nn::hid::NpadBaseState* src) {
            if (!isRecordActive) { } // no return, currentInput still feeds input display
            // XXX copy these source pointers so we can display them in InputDisplay?
            // XXX should i just poll input on calc instead? i'm not confident hooks will always be getting called concurrently with calc()
            std::memcpy((void*)&(currentInput.buttons), (void*)&(src->mButtons), 24);
        }

        inline static void applyCurrentGyro(nn::hid::SixAxisSensorState* src_gyro) {
            if (!isRecordActive) { } // no return, currentInput still feeds input display
            std::memcpy((void*)&(currentInput.gyro), src_gyro, sizeof(nn::hid::SixAxisSensorState));
        }

        inline static void trashToggleDump() {
            isRecordActive = !isRecordActive;
            if (isRecordActive) {
                // init record
                // TODO accumulatorInput = currentInput = {0}?
                accumulatedInput60 = 0;
            } else {
                // TODO close+emit final accumulatorInput?
            }
        }

        private:
        static bool isEquivalent(RecordInput* a, RecordInput* b);
        static void dumpCompletedInput(RecordInput* output, u32 outputDuration60);
    };
} // ns
