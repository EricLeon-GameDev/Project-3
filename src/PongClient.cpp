#include "PongClient.h"
#include "Constants.h"

void PongClient::begin() {
    inputManager.begin();
    renderer.begin();
    ble.begin();
    wifi.begin();

    Serial.println("[BOOT] Running as CLIENT");
    Serial.printf("[BOOT] BLE name: %s\n", BLE_CLIENT_NAME);
    Serial.printf("[BOOT] WiFi label: %s\n", wifi.getDeviceLabel());
}

void PongClient::update() {
    M5.update();
    ble.update();
    wifi.update();

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
}
