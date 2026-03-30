#pragma once
#include <M5Core2.h>
#include "Entities.h"

class InputManager {
public:
    void begin();
    LocalInput readInput(bool useTouch = true);

private:
    bool touchPressedInButtonArea();
};