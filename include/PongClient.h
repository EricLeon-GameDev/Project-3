#pragma once
#include "InputManager.h"
#include "Renderer.h"
#include "BleManager.h"
#include "WifiManager.h"

class PongClient {
public:
    void begin();
    void update();

private:
    InputManager inputManager;
    Renderer renderer;
    BleManager ble;
    WifiManager wifi;

    unsigned long lastInputSendMs = 0;
    GameStatePacket clientViewState{};
};
