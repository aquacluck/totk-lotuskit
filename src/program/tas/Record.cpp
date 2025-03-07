#include <string>
#include <nn/util.h>
#include "tas/config.hpp"
#include "tas/Record.hpp"
#include "structs/VFRMgr.hpp"
#include "Logger.hpp"
#include "TextWriter.hpp"
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
        lotuskit::TextWriter::printf(1, "[tas::Record] ws dump\n");

        VFRMgr* vfrMgr = *EXL_SYM_RESOLVE<VFRMgr**>("engine::module::VFRMgr::sInstance");
        u32 deltaFrame60 = (u32)(vfrMgr->mDeltaFrame * 2);
        // assert(deltaFrame60 == 2 or 3); assert(mDeltaFrame == 1.0 or 1.5 @30fps);

        if (isEquivalent(&currentInput, &accumulatorInput)) {
            // accumulatorInput still held
            accumulatedInput60 += deltaFrame60;
            return;
        }

        // move accumulator to tmp output
        u32 outputDuration60 = accumulatedInput60; // XXX or + deltaFrame60 here instead of giving it to next input?
        RecordInput output = {0}; // lol
        std::memcpy((void*)&(output.buttons), (void*)&(accumulatorInput.buttons), 24);

        // reset accumulator to currentInput
        accumulatedInput60 = deltaFrame60; // XXX or 0 instead of carrying deltaFrame60 over?
        std::memcpy((void*)&(accumulatorInput.buttons), (void*)&(currentInput.buttons), 24);

        dumpCompletedInput(&output, outputDuration60);
    }

    void Record::dumpCompletedInput(RecordInput* output, u32 outputDuration60) {
        if (outputDuration60 == 0) { return; }
        u64 buttons = *(u64*)&(output->buttons);

        u32 outputDurationLogicalFrames = 0;
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            outputDurationLogicalFrames = outputDuration60;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            outputDurationLogicalFrames = outputDuration60 / 2; // XXX do we want floor/ceil/??? for odds
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            outputDurationLogicalFrames = outputDuration60 / 3; // XXX what if not divisible by 3?
        } else return;

        // TODO how/should we dump gyro? definitely not by default, and it's gonna be extremely noisy the entire time it's enabled,
        //      it interferes with input accumulation, etc... I'd rather not try to support recording this tbh

        // TODO option to queue to bg file writer thread, instead of wasting time with socket send?
        //constexpr bool useFormat_nxTASAll = false; // TODO option for pure nx-TAS output? tsv?
        constexpr bool useFormat_nxTASButtons = true;
        if (useFormat_nxTASButtons) {
            std::string buttonsStr = "";
            if (buttons & (1 << 0)) { buttonsStr += "|KEY_A"; }
            if (buttons & (1 << 1)) { buttonsStr += "|KEY_B"; }
            if (buttons & (1 << 2)) { buttonsStr += "|KEY_X"; }
            if (buttons & (1 << 3)) { buttonsStr += "|KEY_Y"; }
            if (buttons & (1 << 6)) { buttonsStr += "|KEY_L"; }
            if (buttons & (1 << 7)) { buttonsStr += "|KEY_R"; }
            if (buttons & (1 << 8)) { buttonsStr += "|KEY_ZL"; }
            if (buttons & (1 << 9)) { buttonsStr += "|KEY_ZR"; }
            if (buttons & (1 << 10)) { buttonsStr += "|KEY_PLUS"; }
            if (buttons & (1 << 11)) { buttonsStr += "|KEY_MINUS"; }
            if (buttons & (1 << 12)) { buttonsStr += "|KEY_DLEFT"; }
            if (buttons & (1 << 13)) { buttonsStr += "|KEY_DUP"; }
            if (buttons & (1 << 14)) { buttonsStr += "|KEY_DRIGHT"; }
            if (buttons & (1 << 15)) { buttonsStr += "|KEY_DDOWN"; }
            if (buttons & (1 << 4)) { buttonsStr += "|KEY_LSTICK"; }
            if (buttons & (1 << 5)) { buttonsStr += "|KEY_RSTICK"; }
            if (buttonsStr.size() == 0) { buttonsStr = " NONE"; }

            char buf[1000];
            nn::util::SNPrintf(buf, sizeof(buf), "tas::input(%d, %s, %d, %d, %d, %d);\n", outputDurationLogicalFrames, buttonsStr.c_str()+1, output->LStick.mX, output->LStick.mY, output->RStick.mX, output->RStick.mY);
            Logger::logText(buf, "/tas/Record");
        } else {
            char buf[1000];
            nn::util::SNPrintf(buf, sizeof(buf), "tas::input(%d, %lld, %d, %d, %d, %d);\n", outputDurationLogicalFrames, buttons, output->LStick.mX, output->LStick.mY, output->RStick.mX, output->RStick.mY);
            Logger::logText(buf, "/tas/Record");
        }
    }
} // ns
