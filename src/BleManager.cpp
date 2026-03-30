#include "BleManager.h"
#include "Constants.h"

BleManager* BleManager::instance = nullptr;

void BleManager::ServerCallbacks::onConnect(NimBLEServer* pServer) {
    owner->connected = true;
    Serial.println("[BLE] Client connected");
}

void BleManager::ServerCallbacks::onDisconnect(NimBLEServer* pServer) {
    owner->connected = false;
    Serial.println("[BLE] Client disconnected");
    NimBLEDevice::startAdvertising();
}

void BleManager::InputCallbacks::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.size() == sizeof(ClientInputPacket)) {
        memcpy(&owner->latestClientInput, value.data(), sizeof(ClientInputPacket));
        owner->newClientInput = true;
    }
}

void BleManager::notifyStateCallback(
    NimBLERemoteCharacteristic* pRemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify
) {
    if (!instance) return;
    if (length == sizeof(GameStatePacket)) {
        memcpy(&instance->latestGameState, pData, sizeof(GameStatePacket));
        instance->newGameState = true;
    }
}

void BleManager::begin() {
    instance = this;
    NimBLEDevice::init("Pong-M5Core2");

#if DEVICE_ROLE == ROLE_HOST
    beginHost();
#else
    beginClient();
#endif
}

void BleManager::beginHost() {
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));

    pService = pServer->createService(SERVICE_UUID);

    pInputChar = pService->createCharacteristic(
        INPUT_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    pInputChar->setCallbacks(new InputCallbacks(this));

    pStateChar = pService->createCharacteristic(
        STATE_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pService->start();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->start();

    Serial.println("[BLE] Host advertising");
}

void BleManager::beginClient() {
    tryConnectClient();
}

void BleManager::tryConnectClient() {
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(true);
    NimBLEScanResults results = scan->start(3, false);

    for (int i = 0; i < results.getCount(); i++) {
        NimBLEAdvertisedDevice d = results.getDevice(i);
        if (d.isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
            pClient = NimBLEDevice::createClient();
            if (!pClient->connect(&d)) {
                Serial.println("[BLE] Client connect failed");
                return;
            }

            pRemoteService = pClient->getService(SERVICE_UUID);
            if (!pRemoteService) {
                Serial.println("[BLE] Service not found");
                pClient->disconnect();
                return;
            }

            pRemoteInputChar = pRemoteService->getCharacteristic(INPUT_CHAR_UUID);
            pRemoteStateChar = pRemoteService->getCharacteristic(STATE_CHAR_UUID);

            if (!pRemoteInputChar || !pRemoteStateChar) {
                Serial.println("[BLE] Characteristics missing");
                pClient->disconnect();
                return;
            }

            if (pRemoteStateChar->canNotify()) {
                pRemoteStateChar->subscribe(true, notifyStateCallback);
            }

            connected = true;
            Serial.println("[BLE] Client connected to host");
            return;
        }
    }

    connected = false;
    Serial.println("[BLE] No host found");
}

void BleManager::update() {
#if DEVICE_ROLE == ROLE_CLIENT
    if (!pClient || !pClient->isConnected()) {
        connected = false;
        tryConnectClient();
    } else {
        connected = true;
    }
#endif
}

bool BleManager::isConnected() const {
    return connected;
}

bool BleManager::hasNewClientInput() const {
    return newClientInput;
}

ClientInputPacket BleManager::getLatestClientInput() {
    newClientInput = false;
    return latestClientInput;
}

void BleManager::sendGameState(const GameStatePacket& state) {
#if DEVICE_ROLE == ROLE_HOST
    if (!pStateChar || !connected) return;
    pStateChar->setValue((uint8_t*)&state, sizeof(GameStatePacket));
    pStateChar->notify();
#endif
}

void BleManager::sendClientInput(const ClientInputPacket& input) {
#if DEVICE_ROLE == ROLE_CLIENT
    if (!pRemoteInputChar || !connected) return;
    pRemoteInputChar->writeValue((uint8_t*)&input, sizeof(ClientInputPacket), false);
#endif
}

bool BleManager::hasNewGameState() const {
    return newGameState;
}

GameStatePacket BleManager::getLatestGameState() {
    newGameState = false;
    return latestGameState;
}