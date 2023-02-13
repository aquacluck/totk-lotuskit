#pragma once

#include "imgui.h"

#include "devgui/DevGuiSettings.h"

#include "devgui/homemenu/HomeMenuBase.h"

class HomeMenuCredits : public HomeMenuBase {
public:
    HomeMenuCredits(DevGuiManager* parent, const char* menuName, sead::Heap* heap);

    virtual void updateMenu();
    // Unlike categories, these don't have a seperation between run and display since code is triggered once on input (or sets flags)
};