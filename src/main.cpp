#include <Arduino.h>
#include <M5Core2.h>
#include "Constants.h"
#include "Entities.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Game.h"
#include "BleManager.h"

InputManager inputManager;
Renderer renderer;
Game game;
BleManager ble;

unsigned long lastStateSendMs = 0;
unsigned long lastInputSendMs = 0;

GameStatePacket clientViewState{};
ClientInputPacket latestClientInput{};

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    Serial.begin(115200);
    delay(500);

    inputManager.begin();
    renderer.begin();
    ble.begin();

#if DEVICE_ROLE == ROLE_HOST
    game.begin();
    Serial.println("[BOOT] Running as HOST");
#else
    Serial.println("[BOOT] Running as CLIENT");
#endif
}

void loop() {
    M5.update();
    ble.update();

#if DEVICE_ROLE == ROLE_HOST
    LocalInput hostInput = inputManager.readInput(true);

    if (ble.hasNewClientInput()) {
        latestClientInput = ble.getLatestClientInput();
    }

    game.updateHost(hostInput, latestClientInput, ble.isConnected());

    unsigned long now = millis();
    if (now - lastStateSendMs >= HOST_STATE_SEND_MS) {
        lastStateSendMs = now;
        GameStatePacket state = game.getStatePacket(ble.isConnected());
        ble.sendGameState(state);
        renderer.draw(state, true, ble.isConnected());
    }

#else
    LocalInput local = inputManager.readInput(true);

    ClientInputPacket out{};
    out.paddleDelta = local.paddleDelta;
    out.abilityPressed = local.abilityPressed;
    out.menuSelectPressed = local.menuSelectPressed;
    out.readyPressed = local.readyPressed;
    out.touchPressed = local.touchPressed;

    unsigned long now = millis();
    if (now - lastInputSendMs >= CLIENT_INPUT_SEND_MS) {
        lastInputSendMs = now;
        ble.sendClientInput(out);
    }

    if (ble.hasNewGameState()) {
        clientViewState = ble.getLatestGameState();
    }

    if (!ble.isConnected()) {
        renderer.drawWaitingForConnection(false, false);
    } else {
        renderer.draw(clientViewState, false, true);
    }
#endif

    delay(5);
}