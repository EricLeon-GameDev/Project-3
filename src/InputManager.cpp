#include "InputManager.h"

void InputManager::begin() {
    // Nothing extra required for built-in buttons/touch.
}

bool InputManager::touchPressedInButtonArea() {
    if (!M5.Touch.ispressed()) return false;
    TouchPoint_t tp = M5.Touch.getPressPoint();
    // Very loose "touch anywhere meaningful" check.
    return (tp.x >= 0 && tp.x <= 320 && tp.y >= 0 && tp.y <= 240);
}

LocalInput InputManager::readInput(bool useTouch) {
    LocalInput in{};

    // Using built-in M5 buttons for simplest working demo:
    // BtnA = up, BtnB = down, BtnC = ability/select/ready
    if (M5.BtnA.wasPressed()) {
        in.paddleDelta = -1;
    } else if (M5.BtnB.wasPressed()) {
        in.paddleDelta = +1;
    }

    if (M5.BtnC.wasPressed()) {
        in.abilityPressed = true;
        in.menuSelectPressed = true;
        in.readyPressed = true;
    }

    if (useTouch && touchPressedInButtonArea()) {
        in.touchPressed = true;
        in.menuSelectPressed = true;
        in.readyPressed = true;
    }

    return in;
}