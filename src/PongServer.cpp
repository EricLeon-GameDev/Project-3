#include "PongServer.h"
#include "Constants.h"

void PongServer::begin() {
    inputManager.begin();
    renderer.begin();
    ble.begin();
    wifi.begin();
    game.begin();

    Serial.println("[BOOT] Running as SERVER / HOST");
    Serial.printf("[BOOT] BLE name: %s\n", BLE_HOST_NAME);
    Serial.printf("[BOOT] WiFi label: %s\n", wifi.getDeviceLabel());
}

void PongServer::update() {
    M5.update();
    ble.update();
    wifi.update();

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
}
