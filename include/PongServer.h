#pragma once
#include "InputManager.h"
#include "Renderer.h"
#include "Game.h"
#include "BleManager.h"
#include "WifiManager.h"

class PongServer {
public:
    void begin();
    void update();

private:
    InputManager inputManager;
    Renderer renderer;
    Game game;
    BleManager ble;
    WifiManager wifi;

    unsigned long lastStateSendMs = 0;
    ClientInputPacket latestClientInput{};
};
