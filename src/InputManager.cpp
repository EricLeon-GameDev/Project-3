#include "InputManager.h"
#include "Constants.h"

void InputManager::begin() {
}

bool InputManager::touchPressedAnywhere() {
    return M5.Touch.ispressed();
}

LocalInput InputManager::readInput(bool useTouch) {
    LocalInput in{};

    if (M5.BtnA.isPressed()) {
        in.paddleDelta = -1;
    } else if (M5.BtnB.isPressed()) {
        in.paddleDelta = 1;
    }

    if (M5.BtnC.wasPressed()) {
        in.abilityPressed = true;
        in.menuSelectPressed = true;
        in.readyPressed = true;
    }

    if (useTouch && touchPressedAnywhere()) {
        TouchPoint_t tp = M5.Touch.getPressPoint();
        int screenCenter = SCREEN_H / 2;

        if (tp.y < screenCenter - TOUCH_MOVE_DEAD_ZONE) {
            in.paddleDelta = -1;
        } else if (tp.y > screenCenter + TOUCH_MOVE_DEAD_ZONE) {
            in.paddleDelta = 1;
        }

        in.touchPressed = true;
        in.menuSelectPressed = true;
        in.readyPressed = true;
    }

    return in;
}
