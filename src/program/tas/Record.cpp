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
        accumulatedRecord60 += deltaFrame60;

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

        emitCompletedInput(&output, outputDuration60);
    }

    void Record::beginDumpImpl() {
        accumulatedInput60 = 0;
        accumulatedRecord60 = 0;
        isRecordActive = true;
        Logger::logJson(json::object({
            {"beginDump", true} // announce
        }), "/tas/Record", false, false); // noblock, no debug log
    }

    void Record::endDump() {
        // FIXME emit final input
        isRecordActive = false;
        Logger::logJson(json::object({
            {"endDump", true} // announce
        }), "/tas/Record", false, false); // noblock, no debug log
    }

    void Record::emitCompletedInput(RecordInput* output, u32 outputDuration60) {
        if (outputDuration60 == 0) { return; }
        u64 buttons = *(u64*)&(output->buttons);

        // scale for assumed fps
        u32 outputDurationLogicalFrames = 0;
        u32 outputTimestampLogicalFrames = 0;
        if (config::inputMode == config::InputDurationScalingStrategy::FPS60_1X) {
            outputDurationLogicalFrames = outputDuration60;
            outputTimestampLogicalFrames = accumulatedRecord60;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS30_2X) {
            outputDurationLogicalFrames = outputDuration60 / 2; // XXX do we want floor/ceil/??? for odds
            outputTimestampLogicalFrames = accumulatedRecord60 / 2;
        } else if (config::inputMode == config::InputDurationScalingStrategy::FPS20_3X) {
            outputDurationLogicalFrames = outputDuration60 / 3; // XXX what if not divisible by 3?
            outputTimestampLogicalFrames = accumulatedRecord60 / 3;
        } else return;

        // format line, send it to emitInputImpl
        char buf[1000];
        if (useFormat_nxTASButtons || useFormat_nxTASAll) {
            std::string buttonsSep = useFormat_nxTASAll ? ";" : "|";
            std::string buttonsStr = "";
            if (buttons & (1 << 0)) { buttonsStr += buttonsSep + "KEY_A"; }
            if (buttons & (1 << 1)) { buttonsStr += buttonsSep + "KEY_B"; }
            if (buttons & (1 << 2)) { buttonsStr += buttonsSep + "KEY_X"; }
            if (buttons & (1 << 3)) { buttonsStr += buttonsSep + "KEY_Y"; }
            if (buttons & (1 << 6)) { buttonsStr += buttonsSep + "KEY_L"; }
            if (buttons & (1 << 7)) { buttonsStr += buttonsSep + "KEY_R"; }
            if (buttons & (1 << 8)) { buttonsStr += buttonsSep + "KEY_ZL"; }
            if (buttons & (1 << 9)) { buttonsStr += buttonsSep + "KEY_ZR"; }
            if (buttons & (1 << 10)) { buttonsStr += buttonsSep + "KEY_PLUS"; }
            if (buttons & (1 << 11)) { buttonsStr += buttonsSep + "KEY_MINUS"; }
            if (buttons & (1 << 12)) { buttonsStr += buttonsSep + "KEY_DLEFT"; }
            if (buttons & (1 << 13)) { buttonsStr += buttonsSep + "KEY_DUP"; }
            if (buttons & (1 << 14)) { buttonsStr += buttonsSep + "KEY_DRIGHT"; }
            if (buttons & (1 << 15)) { buttonsStr += buttonsSep + "KEY_DDOWN"; }
            if (buttons & (1 << 4)) { buttonsStr += buttonsSep + "KEY_LSTICK"; }
            if (buttons & (1 << 5)) { buttonsStr += buttonsSep + "KEY_RSTICK"; }
            if (buttonsStr.size() == 0) { buttonsStr = " NONE"; }

            if (useFormat_nxTASAll) {
                // nx-TAS
                for (u32 fr = outputTimestampLogicalFrames - outputDurationLogicalFrames; fr < outputTimestampLogicalFrames; fr++) {
                    // one line per frame, un-accumulated
                    nn::util::SNPrintf(buf, sizeof(buf), "%d %s %d;%d %d;%d\n", fr, buttonsStr.c_str()+1, output->LStick.mX, output->LStick.mY, output->RStick.mX, output->RStick.mY);
                    emitInputImpl(buf);
                }

            } else {
                // AS with enum buttons
                nn::util::SNPrintf(buf, sizeof(buf), "tas::input(%d, %s, %d, %d, %d, %d);\n", outputDurationLogicalFrames, buttonsStr.c_str()+1, output->LStick.mX, output->LStick.mY, output->RStick.mX, output->RStick.mY);
                emitInputImpl(buf);
            }

        } else {
            // AS with raw int buttons
            nn::util::SNPrintf(buf, sizeof(buf), "tas::input(%d, %lld, %d, %d, %d, %d);\n", outputDurationLogicalFrames, buttons, output->LStick.mX, output->LStick.mY, output->RStick.mX, output->RStick.mY);
            emitInputImpl(buf);
        }
    }

    void Record::emitInputImpl(const char* line) {
        if (doEmitWS) {
            Logger::logText(line, "/tas/Record", false, doEmitDebug);
        } else if (doEmitDebug) {
            svcOutputDebugString(line, strlen(line));
        }
        if (doEmitFile) {
            // TODO write to useEmitLocalFile
        }
    }

} // ns
