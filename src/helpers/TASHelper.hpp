#pragma once
#include <atomic>
#include <cstring>
#include "nn/fs.h"
#include "nn/hid.h"
#include "math/seadVector.h"

namespace lotuskit::tas {

class ScriptElement {
    public:
    // 8B meta
    u16 elementType; // = 0 nop/skip
    u16 _pad1[3];
    u8 _pad2[24]; // 24B payload
};
class ScriptElement_NpadSimple {
    public:
    // 8B meta
    u16 elementType; // = 1
    u16 durationFrames60; // ie integer half-frames at 30fps -- durationFrames60=3 means one 20fps frame
    u16 _pad1[2];

    // 24B payload
    nn::hid::NpadButtonSet buttons; // u64
    nn::hid::AnalogStickState LStick; // s32 s32
    nn::hid::AnalogStickState RStick; // s32 s32

    // u64 _pad2[4]; //XXX might need more padding to eg set angles, gyro, pass pointers/positions/???

    inline void apply(nn::hid::NpadBaseState* dst) {
        std::memcpy((void*)&(dst->mButtons), (void*)&(buttons), 24);
    }
};
class ScriptElement_SetPlayerPos {
    public:
    // 8B meta
    u16 elementType; // = 2
    u16 _pad1[3];

    // 24B payload
    sead::Vector3f pos; // 12B
    sead::Vector3f _pad; // 12B
    // TODO rotation, climb/etc states, etc? can also split out elements 
};
static_assert(32 == sizeof(ScriptElement)); // ScriptElement classes effectively form a union
static_assert(32 == sizeof(ScriptElement_NpadSimple));
static_assert(32 == sizeof(ScriptElement_SetPlayerPos));

class Script_Playback_Simple {
    public:
    ScriptElement* begin;
    size_t durationElements;
    u32 element_i;
    u16 element_elapsedFrames60;
    bool do_repeat;

    ScriptElement* proc_consume_frames(u16* deltaFrames60);

    void load_dummy_script(void);
};

class Script_Playback_RingBuffer_FileBacked {
    public:
    constexpr static size_t BUF_ELEMENTS = 30 * 60; // eg 1min of unique 30fps
    ScriptElement buf[BUF_ELEMENTS];
    u16 head_i; // current+earliest+lowest element: playback always injects whatever this points at
    u16 head_elapsedFrames60;
    u16 tail_i; // newest streamed from file: elements are emplaced forward from here
    bool do_repeat;

    ScriptElement* proc_consume_frames(u16* deltaFrames60);

    void load_dummy_script(void);
    void load_dummy_script_more(void);
};

class TASHelper {
    public:
    static void proc_playback(float deltaFrame);
    //static void proc_record(float deltaFrame);

    //private:
    static Script_Playback_RingBuffer_FileBacked script_file;
    static Script_Playback_Simple script_simple;

    static inline std::atomic<ScriptElement_NpadSimple*> playback_current = nullptr;
    static inline bool do_playback_file = false;
    static inline bool do_playback_simple = false;
    //static inline bool do_main_world_proc_record = false;
};

}
