#include "InputHelper.hpp"
//#include "diag/assert.hpp"

//#include "devgui/DevGuiManager.h"

static const char *styleNames[] = {
        "Pro Controller",
        "Joy-Con controller in handheld mode",
        "Joy-Con controller in dual mode",
        "Joy-Con left controller in single mode",
        "Joy-Con right controller in single mode",
        "GameCube controller",
        "Poké Ball Plus controller",
        "NES/Famicom controller",
        "NES/Famicom controller in handheld mode",
        "SNES controller",
        "N64 controller",
        "Sega Genesis controller",
        "generic external controller",
        "generic controller",
};


inline bool isBitSet(nn::hid::NpadStyleSet style, nn::hid::NpadStyleTag index) {
    return (style._storage[static_cast<u64>(index) / style.StorageBitCount] &
           (static_cast<uint32_t>(1) << static_cast<u64>(index) % style.StorageBitCount)) != 0;
}
inline bool isBitSet(nn::hid::NpadButtonSet style, nn::hid::NpadButton index) {
    return (style._storage[static_cast<u64>(index) / style.StorageBitCount] &
           (static_cast<uint64_t>(1) << static_cast<u64>(index) % style.StorageBitCount)) != 0;
}
inline bool isBitSet(nn::util::BitFlagSet<256, nn::hid::KeyboardKey> style, nn::hid::KeyboardKey index) {
    return (style._storage[static_cast<u64>(index) / style.StorageBitCount] &
           (static_cast<uint64_t>(1) << static_cast<u64>(index) % style.StorageBitCount)) != 0;
}
inline bool isBitSet(nn::hid::MouseButtonSet style, nn::hid::MouseButton index) {
    return (style._storage[static_cast<u64>(index) / style.StorageBitCount] &
           (static_cast<uint32_t>(1) << static_cast<u64>(index) % style.StorageBitCount)) != 0;
}


nn::hid::NpadBaseState InputHelper::prevControllerState{};
nn::hid::NpadBaseState InputHelper::curControllerState{};

nn::hid::KeyboardState InputHelper::curKeyboardState{};
nn::hid::KeyboardState InputHelper::prevKeyboardState{};

nn::hid::MouseState InputHelper::curMouseState{};
nn::hid::MouseState InputHelper::prevMouseState{};

ulong InputHelper::selectedPort = -1;
bool InputHelper::isReadInput = true;
bool InputHelper::toggleInput = false;
bool InputHelper::enableScroll = true;

const char *getStyleName(nn::hid::NpadStyleSet style) {

    s32 index = -1;

    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleFullKey)) { index = 0; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleHandheld)) { index = 1; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleJoyDual)) { index = 2; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleJoyLeft)) { index = 3; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleJoyRight)) { index = 4; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleGc)) { index = 5; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStylePalma)) { index = 6; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleLark)) { index = 7; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleHandheldLark)) { index = 8; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleLucia)) { index = 9; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleLagon)) { index = 10; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleLager)) { index = 11; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleSystemExt)) { index = 12; }
    if (isBitSet(style, nn::hid::NpadStyleTag::NpadStyleSystem)) { index = 13; }

    if (index != -1) {
        return styleNames[index];
    } else {
        return "Unknown";
    }
}

void InputHelper::initKBM() {
    nn::hid::InitializeKeyboard();
    nn::hid::InitializeMouse();
}

void InputHelper::updatePadState() {
    setIsHandheldMode();

    prevControllerState = curControllerState;
    tryGetContState(&curControllerState, selectedPort);

    prevKeyboardState = curKeyboardState;
    nn::hid::GetKeyboardState(&curKeyboardState);

    prevMouseState = curMouseState;
    nn::hid::GetMouseState(&curMouseState);

    if (isHoldR() && isHoldZR() && isPressZL()) {
        toggleInput = !toggleInput;
    }

    //if (!DevGuiManager::instance()->isMenuActive())
    //    toggleInput = false;
}

bool InputHelper::tryGetContState(nn::hid::NpadBaseState *state, ulong port) {

    nn::hid::NpadStyleSet styleSet = nn::hid::GetNpadStyleSet(port);
    isReadInput = true;
    bool result = true;

    if (isBitSet(styleSet, nn::hid::NpadStyleTag::NpadStyleFullKey)) {
        nn::hid::GetNpadState((nn::hid::NpadFullKeyState *) state, port);
    } else if (isBitSet(styleSet, nn::hid::NpadStyleTag::NpadStyleHandheld)) {
        nn::hid::GetNpadState((nn::hid::NpadHandheldState *) state, port);
    } else if (isBitSet(styleSet, nn::hid::NpadStyleTag::NpadStyleJoyDual)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyDualState *) state, port);
    } else if (isBitSet(styleSet, nn::hid::NpadStyleTag::NpadStyleJoyLeft)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyLeftState *) state, port);
    } else if (isBitSet(styleSet, nn::hid::NpadStyleTag::NpadStyleJoyRight)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyRightState *) state, port);
    } else {
        result = false;
    }

    isReadInput = false;

    return result;

}

void InputHelper::setIsHandheldMode()
{
    setPort(0);

    nn::hid::NpadStyleSet style = nn::hid::GetNpadStyleSet(0); // Gets player 1's controller style
    // If no controller is connected in port 0, migrate selected port to handheld 0x20

    if(isBitSet(style, nn::hid::NpadStyleTag::NpadStyleFullKey)) return;
    if(isBitSet(style, nn::hid::NpadStyleTag::NpadStyleJoyDual)) return;
    if(isBitSet(style, nn::hid::NpadStyleTag::NpadStyleJoyLeft)) return;
    if(isBitSet(style, nn::hid::NpadStyleTag::NpadStyleJoyRight)) return;

    setPort(0x20);
}

bool InputHelper::isButtonHold(nn::hid::NpadButton button) {
    return isBitSet(curControllerState.mButtons, button);
}

bool InputHelper::isButtonPress(nn::hid::NpadButton button) {
    return isBitSet(curControllerState.mButtons, button) && !isBitSet(prevControllerState.mButtons, button);
}

bool InputHelper::isButtonRelease(nn::hid::NpadButton button) {
    return !isBitSet(curControllerState.mButtons, button) && isBitSet(prevControllerState.mButtons, button);
}

bool InputHelper::isKeyHold(nn::hid::KeyboardKey key) {
    return isBitSet(curKeyboardState.mKeys, key);
}

bool InputHelper::isKeyPress(nn::hid::KeyboardKey key) {
    return isBitSet(curKeyboardState.mKeys, key) && !isBitSet(prevKeyboardState.mKeys, key);
}

bool InputHelper::isKeyRelease(nn::hid::KeyboardKey key) {
    return !isBitSet(curKeyboardState.mKeys, key) && isBitSet(prevKeyboardState.mKeys, key);
}

bool InputHelper::isMouseHold(nn::hid::MouseButton button) {
    return isBitSet(curMouseState.buttons, button);
}

bool InputHelper::isMousePress(nn::hid::MouseButton button) {
    return isBitSet(curMouseState.buttons, button) && !isBitSet(prevMouseState.buttons, button);
}

bool InputHelper::isMouseRelease(nn::hid::MouseButton button) {
    return !isBitSet(curMouseState.buttons, button) && isBitSet(prevMouseState.buttons, button);
}

void InputHelper::getMouseCoords(float *x, float *y) {
    *x = curMouseState.x;
    *y = curMouseState.y;
}

void InputHelper::getScrollDelta(float *x, float *y) {
    if (InputHelper::canScroll()) {
        *x = curMouseState.wheelDeltaX;
        *y = curMouseState.wheelDeltaY;
    } else {
        *x = 0.f;
        *y = 0.f;
    }
}
