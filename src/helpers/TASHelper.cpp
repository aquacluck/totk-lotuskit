#include "helpers/TASHelper.hpp"
#include "logger/Logger.hpp"

namespace lotuskit::tas {

Script_Playback_RingBuffer_FileBacked TASHelper::script_file = {};
Script_Playback_Simple TASHelper::script_simple = {};

void Script_Playback_Simple::load_dummy_script() {
    Logger::main->log(NS_TAS, R"("load_dummy_script")");

    durationElements = 4;
    begin = new ScriptElement[durationElements];
    element_i = 0;
    element_elapsedFrames60 = 0;
    do_repeat = true;

    auto* head_npad = (ScriptElement_NpadSimple*)&(begin[element_i]);
    head_npad->elementType = 1;
    head_npad->durationFrames60 = 60*1; // 1s
    *((u64*)&(head_npad->buttons)) = 1 << (u64)nn::hid::NpadButton::A | 1 << (u64)nn::hid::NpadButton::StickLDown;
    head_npad->LStick.mX = 0;
    head_npad->LStick.mY = -32000;
    head_npad->RStick.mX = 0;
    head_npad->RStick.mY = 0;

    head_npad = (ScriptElement_NpadSimple*)&(begin[element_i+1]);
    head_npad->elementType = 1;
    head_npad->durationFrames60 = 60*1; // 1s
    *((u64*)&(head_npad->buttons)) = 1 << (u64)nn::hid::NpadButton::StickLUp; // "buttons" for axes should be set for input display, but otherwise dont seem to matter. sdk flips them on at 16400.
    head_npad->LStick.mX = 0;
    head_npad->LStick.mY = 32000;
    head_npad->RStick.mX = 0;
    head_npad->RStick.mY = 0;

    head_npad = (ScriptElement_NpadSimple*)&(begin[element_i+2]);
    head_npad->elementType = 1;
    head_npad->durationFrames60 = 60*1; // 1s
    *((u64*)&(head_npad->buttons)) = 1 << (u64)nn::hid::NpadButton::StickLDown;
    head_npad->LStick.mX = 0;
    head_npad->LStick.mY = -32000;
    head_npad->RStick.mX = 0;
    head_npad->RStick.mY = 0;

    head_npad = (ScriptElement_NpadSimple*)&(begin[element_i+3]);
    head_npad->elementType = 1;
    head_npad->durationFrames60 = 60*1; // 1s
    *((u64*)&(head_npad->buttons)) = 1 << (u64)nn::hid::NpadButton::StickLUp;
    head_npad->LStick.mX = 0;
    head_npad->LStick.mY = 32000;
    head_npad->RStick.mX = 0;
    head_npad->RStick.mY = 0;

    // FIXME scripts need to be loaded somewhere that can dispatch any leading commands, or do it in first proc_playback when nullptr --
    // playback_current must not point to these leading non-input elements
    TASHelper::playback_current = (ScriptElement_NpadSimple*)&(begin[element_i]);
}

void Script_Playback_RingBuffer_FileBacked::load_dummy_script() { } // TODO
void Script_Playback_RingBuffer_FileBacked::load_dummy_script_more() { // TODO
    //Logger::main->log(NS_TAS, R"("load_dummy_script_more")");
    // assert load size 100 < minimum available when called (at runway=30 and BUF_ELEMENTS=180, 150)
    //script.tail_i = (script.tail_i + 100) % Script_Playback_RingBuffer_FileBacked::BUF_ELEMENTS;
}

ScriptElement* Script_Playback_Simple::proc_consume_frames(u16* deltaFrames60) {
    ScriptElement_NpadSimple* element = (ScriptElement_NpadSimple*)(begin+element_i);
    u16 element_remaining60 = element->elementType == 1 ? element->durationFrames60 - element_elapsedFrames60 : 0;

    if (*deltaFrames60 < element_remaining60) {
        // continue this input element
        element_elapsedFrames60 += *deltaFrames60;
        *deltaFrames60 = 0;
        return nullptr; // did not advance
    }

    // element complete
    *deltaFrames60 -= element_remaining60;
    element_i++;
    if (element_i >= durationElements && do_repeat) {
        // repeat
        element_i = 0;
        Logger::main->log(NS_TAS, R"("script_simple repeat")");
    } else if (element_i >= durationElements && !do_repeat) {
        // end playback
        TASHelper::do_playback_simple = false;
        TASHelper::playback_current = nullptr;
        Logger::main->log(NS_TAS, R"("script_simple end")");
        return nullptr;
    } else {
        // normal advance element 
        Logger::main->log(NS_TAS, R"("script_simple element_i++")");
    }
    element_elapsedFrames60 = 0;
    return begin+element_i;
}

ScriptElement* Script_Playback_RingBuffer_FileBacked::proc_consume_frames(u16* deltaFrames60) {
    return nullptr; // XXX TODO
}


void TASHelper::proc_playback(float deltaFrame) {
    // tick playback to determine current Npad element
    // assert deltaFrame is always 1.0 or 1.5
    // assert playback_current is always an Npad element
    u16 deltaFrames60 = (u16)(deltaFrame * 2);
    ScriptElement* maybeNextElement = (ScriptElement*)&(*playback_current);

    if (do_playback_simple) {
        while (deltaFrames60 > 0 || (maybeNextElement && maybeNextElement->elementType != 1)) {
            maybeNextElement = script_simple.proc_consume_frames(&deltaFrames60);
            if (!maybeNextElement) { return; } // continue using playback_current

            if (maybeNextElement->elementType == 1) {
                playback_current = (ScriptElement_NpadSimple*)maybeNextElement;
                //Logger::main->logf(NS_TAS, R"("assign playback_current=%p")", maybeNextElement);
                continue; // loop to deduct any remaining time
            }
            
            // TODO dispatch non-input element
        }

    } else if (do_playback_file) {
        // TODO
    }
}

/*
void TASHelper::main_world_proc_playback(float deltaFrame) {
    // TODO lock script

    // tick playback to determine current Npad element
    // assert deltaFrame is always 1.0 or 1.5
    u16 deltaFrames60 = (u16)(deltaFrame * 2);
    script.head_elapsedFrames60 += deltaFrames60;

    // assert head_i is always an Npad element
    ScriptElement* head_element = &(script.buf[script.head_i]);
    if (head_element->elementType != 1) {
        // TODO panic -- head needs to support a duration! instant commands/etc must be consumed and an Npad input element assigned
        return;
    }
    auto* head_npad = (ScriptElement_NpadSimple*)head_element;

    // advance to next element if needed
    // FIXME advance/skip more than one if needed -- the longest deltaFrame 1.5 covers 3 * durationFrames60=1
    if (script.head_elapsedFrames60 >= head_npad->durationFrames60) {
        script.head_elapsedFrames60 -= head_npad->durationFrames60;
        script.head_i++;
        script.elapsedElements++;

        Logger::main->log(NS_TAS, R"("element++")");

        if (script.head_i >= Script_Playback_RingBuffer_FileBacked::BUF_ELEMENTS) {
            script.head_i = 0;
        }

        if (script.head_i == script.tail_i) {
            // TODO panic -- runway exhausted, desync inevitable
            return;
        }

        s32 runway = script.tail_i - script.head_i;
        if (runway < 0) { runway += Script_Playback_RingBuffer_FileBacked::BUF_ELEMENTS; }
        if (runway < 30) {
            // TODO read more from file  
            load_dummy_script_more();
        }

        // TODO execute non-Npad elements
        // head_element = script.buf[script.head_i];
        // while (head_element->elementType != 1) { ... }

        if (script.elapsedElements >= script.durationElements) {
            // playback complete
            do_main_world_proc_playback = false;
        }

    }
}
*/

//void TASHelper::main_world_proc_record(float deltaFrame) { } // TODO

} // ns
