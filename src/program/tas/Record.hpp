#pragma once
#include "exlaunch.hpp"
#include <angelscript.h>
#include <nn/fs.h>
#include <nn/hid.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <string>
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

        // held inputs are "accumulated" into a single tas::input(framecount) line:
        static RecordInput currentInput; // volatile, overwritten constantly by applyCurrentInput hook
        static RecordInput accumulatorInput; // for keeping track of how long currentInput is held
        inline static u32 accumulatedInput60 = 0; // single input duration
        inline static u32 accumulatedRecord60 = 0; // entire recording duration (used for nxtas line labels + modeline framecount)
        inline static bool isRecordActive = false; // calc accumulation+emit

        // completed inputs are emitted according to logging/file/etc options:
        inline static bool doEmitWS = true;
        inline static bool doEmitFile = false;
        inline static std::string useEmitLocalFile = "";
        inline static nn::fs::FileHandle useEmitLocalFileFd = {};
        inline static u32 useEmitLocalFileOffset = 0;
        inline static bool doEmitDebug = false;
        inline static bool useFormat_emitGyro = false; // TODO gyro is extremely noisy
        inline static bool useFormat_nxTASAll = false; // pure nx-TAS output
        inline static bool useFormat_nxTASButtons = true; // use enums (eg KEY_B instead of raw buttons int), produces larger+slower code but u can read it :)
        // TODO option to queue to bg file writer thread?

        // copy the provided state (eg from controller or tas playback), which will be reported as the current input for InputDisplay/Record/etc
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

        // expose some ez presets for AS
        static void beginDumpImpl(); // start new record, reusing current settings
        static void endDump(); // close codegen block/footers, flush+close file, etc
        inline static void beginFileDump(const std::string& filename) {
            doEmitDebug = false;
            doEmitWS = false;
            doEmitFile = true;
            useEmitLocalFile = filename;
            useEmitLocalFileOffset = 0;
            useFormat_emitGyro = false;
            useFormat_nxTASAll = false;
            useFormat_nxTASButtons = true;
            beginDumpImpl();
        }
        inline static void beginFileDumpNXTas(const std::string& filename) {
            doEmitDebug = false;
            doEmitWS = false;
            doEmitFile = true;
            useEmitLocalFile = filename;
            useEmitLocalFileOffset = 0;
            useFormat_emitGyro = false;
            useFormat_nxTASAll = true;
            useFormat_nxTASButtons = true;
            beginDumpImpl();
        }
        inline static void beginLoggerDump(bool ws = true) {
            doEmitDebug = !ws;
            doEmitWS = ws;
            doEmitFile = false;
            useEmitLocalFile = "";
            useEmitLocalFileOffset = 0;
            useFormat_emitGyro = false;
            useFormat_nxTASAll = false;
            useFormat_nxTASButtons = true;
            beginDumpImpl();
        }
        inline static void beginLoggerDumpNXTas(bool ws = true) {
            doEmitDebug = !ws;
            doEmitWS = ws;
            doEmitFile = false;
            useEmitLocalFile = "";
            useEmitLocalFileOffset = 0;
            useFormat_emitGyro = false;
            useFormat_nxTASAll = true;
            useFormat_nxTASButtons = true;
            beginDumpImpl();
        }
        inline static void toggleDump() {
            if (!isRecordActive) {
                beginDumpImpl();
            } else {
                endDump();
            }
        }

        private:
        static bool isEquivalent(RecordInput* a, RecordInput* b);
        static void calcCompletedInput(u32 remainderInit60);
        static void emitCompletedInput(RecordInput* output, u32 outputDuration60);
        static void emitInputImpl(const char* line, u8 chunkOp);
    };
} // ns
