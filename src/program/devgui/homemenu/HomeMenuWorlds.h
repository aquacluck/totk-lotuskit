#pragma once

#include "imgui.h"

#include "game/GameData/GameDataHolder.h"
#include "game/GameData/GameDataFunction.h"

#include "program/GetterUtil.h"

#include "devgui/DevGuiSettings.h"
#include "program/devgui/homemenu/HomeMenuBase.h"

class HomeMenuWorlds : public HomeMenuBase {
public:
    HomeMenuWorlds(const char* menuName, sead::Heap* heap);

    virtual void updateMenu();
    // Unlike categories, these don't have a seperation between run and display since code is triggered once on input (or sets flags)

private:
    void drawScenarioPicker(GameDataHolderAccessor* data, WorldListEntry* entry);
    void warpToStage(GameDataHolderAccessor* data, const char* stageName, int scenario);
};