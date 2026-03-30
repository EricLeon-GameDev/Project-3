#include "BleManager.h"
#include "Constants.h"

BleManager* BleManager::instance = nullptr;

void BleManager::ServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    owner->connected = true;
    Serial.println("[BLE] Client connected");
}

void BleManager::ServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
    owner->connected = false;
    Serial.printf("[BLE] Client disconnected, reason=%d\n", reason);

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    if (advertising) {
        advertising->start();
    }
}

void BleManager::InputCallbacks::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
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

#if DEVICE_ROLE == ROLE_HOST
    NimBLEDevice::init(BLE_HOST_NAME);
    beginHost();
#else
    NimBLEDevice::init(BLE_CLIENT_NAME);
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

    // okay to leave this out in newer NimBLE, but it is harmless if kept
    pService->start();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);

    // Make the Bluetooth name visible
    advertising->enableScanResponse(true);
    advertising->setName(BLE_HOST_NAME);

    advertising->start();

    Serial.printf("[BLE] Host advertising as %s\n", BLE_HOST_NAME);
}

void BleManager::beginClient() {
    tryConnectClient();
}

void BleManager::tryConnectClient() {
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(true);

    // NimBLE 2.x blocking scan flow
    NimBLEScanResults results = scan->getResults(3000);

    for (int i = 0; i < results.getCount(); i++) {
        const NimBLEAdvertisedDevice* d = results.getDevice(i);
        if (!d) continue;

        if (d->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
            pClient = NimBLEDevice::createClient();
            if (!pClient) {
                Serial.println("[BLE] Failed to create client");
                return;
            }

            if (!pClient->connect(d)) {
                Serial.println("[BLE] Client connect failed");
                NimBLEDevice::deleteClient(pClient);
                pClient = nullptr;
                return;
            }

            pRemoteService = pClient->getService(SERVICE_UUID);
            if (!pRemoteService) {
                Serial.println("[BLE] Service not found");
                pClient->disconnect();
                NimBLEDevice::deleteClient(pClient);
                pClient = nullptr;
                return;
            }

            pRemoteInputChar = pRemoteService->getCharacteristic(INPUT_CHAR_UUID);
            pRemoteStateChar = pRemoteService->getCharacteristic(STATE_CHAR_UUID);

            if (!pRemoteInputChar || !pRemoteStateChar) {
                Serial.println("[BLE] Characteristics missing");
                pClient->disconnect();
                NimBLEDevice::deleteClient(pClient);
                pClient = nullptr;
                return;
            }

            if (pRemoteStateChar->canNotify()) {
                pRemoteStateChar->subscribe(true, notifyStateCallback);
            }

            connected = true;
            Serial.printf("[BLE] Client connected to host %s\n", d->getName().c_str());
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