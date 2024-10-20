#include "nn/util.h"
#include "tas/Record.hpp"
#include "syms_merged.hpp"
#include "structs/VFRMgr.hpp"
#include "Logger.hpp"
using Logger = lotuskit::Logger;

namespace lotuskit::tas {
    RecordInput Record::currentInput = {0};
    RecordInput Record::accumulatorInput = {0};

    bool Record::isEquivalent(RecordInput* a, RecordInput* b) {
        // TODO deadzones? optional rounding or snapping? often an approximate capture is good enough to start + smaller
        if(*(u64*)&(a->buttons) != *(u64*)&(b->buttons)) { return false; }
        if(*(u64*)&(a->LStick) != *(u64*)&(b->LStick)) { return false; }
        if(*(u64*)&(a->RStick) != *(u64*)&(b->RStick)) { return false; }
        return true;
    }

    void Record::calc() {
        if (!isRecordActive) { return; } // not doing record

        VFRMgr* vfrMgr = *exl::util::pointer_path::FollowSafe<VFRMgr*, sym::engine::module::VFRMgr::sInstance::offset>();
        u32 deltaFrame60 = (u32)(vfrMgr->mDeltaFrame * 2);
        // assert(deltaFrame60 == 2 or 3); assert(mDeltaFrame == 1.0 or 1.5 @30fps);

        if (isEquivalent(&currentInput, &accumulatorInput)) {
            // accumulatorInput still held
            accumulatedInput60 += deltaFrame60;
            return;
        }

        // move accumulator to tmp output
        u32 outputDuration60 = accumulatedInput60; // + deltaFrame60 FIXME do this?
        RecordInput output = {0}; // lol
        std::memcpy((void*)&(output.buttons), (void*)&(accumulatorInput.buttons), 24);

        // reset accumulator to currentInput
        //accumulatedInput60 = 0;
        accumulatedInput60 = deltaFrame60; // FIXME or do this? or something else entirely?
        // (eg could a separate thread provide reliable hires input timeline? how would we reconcile that w vfr?)
        std::memcpy((void*)&(accumulatorInput.buttons), (void*)&(currentInput.buttons), 24);

        dumpCompletedInput(&output, outputDuration60);
    }

    void Record::dumpCompletedInput(RecordInput* output, u32 outputDuration60) {
        // TODO option for nx-TAS format? or at least KEY_ZL|KEY_X style button repr
        // TODO option to queue to bg file writer thread, instead of wasting time with socket send?
        char buf[1000];
        nn::util::SNPrintf(buf, sizeof(buf), "tas::input(%d, %lld, %d, %d, %d, %d);\n", outputDuration60, *(u64*)&(output->buttons), output->LStick.mX, output->LStick.mY, output->RStick.mX, output->RStick.mY);
        Logger::logText(buf, "/tas/Record");
    }
} // ns
