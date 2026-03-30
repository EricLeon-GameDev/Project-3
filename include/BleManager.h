#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "Entities.h"

#ifndef DEVICE_ROLE
#define DEVICE_ROLE ROLE_HOST
#endif

class BleManager {
public:
    void begin();
    void update();

    bool isConnected() const;

    bool hasNewClientInput() const;
    ClientInputPacket getLatestClientInput();
    void sendGameState(const GameStatePacket& state);

    void sendClientInput(const ClientInputPacket& input);
    bool hasNewGameState() const;
    GameStatePacket getLatestGameState();

private:
    static constexpr const char* SERVICE_UUID    = "12345678-1234-1234-1234-1234567890ab";
    static constexpr const char* INPUT_CHAR_UUID = "12345678-1234-1234-1234-1234567890ac";
    static constexpr const char* STATE_CHAR_UUID = "12345678-1234-1234-1234-1234567890ad";

    NimBLEServer* pServer = nullptr;
    NimBLEService* pService = nullptr;
    NimBLECharacteristic* pInputChar = nullptr;
    NimBLECharacteristic* pStateChar = nullptr;

    NimBLEClient* pClient = nullptr;
    NimBLERemoteService* pRemoteService = nullptr;
    NimBLERemoteCharacteristic* pRemoteInputChar = nullptr;
    NimBLERemoteCharacteristic* pRemoteStateChar = nullptr;

    bool connected = false;
    volatile bool newClientInput = false;
    volatile bool newGameState = false;

    ClientInputPacket latestClientInput{};
    GameStatePacket latestGameState{};

    void beginHost();
    void beginClient();
    void tryConnectClient();

    class ServerCallbacks : public NimBLEServerCallbacks {
    public:
        explicit ServerCallbacks(BleManager* owner) : owner(owner) {}
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
    private:
        BleManager* owner;
    };

    class InputCallbacks : public NimBLECharacteristicCallbacks {
    public:
        explicit InputCallbacks(BleManager* owner) : owner(owner) {}
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
    private:
        BleManager* owner;
    };

    static void notifyStateCallback(
        NimBLERemoteCharacteristic* pRemoteCharacteristic,
        uint8_t* pData,
        size_t length,
        bool isNotify);

    static BleManager* instance;
};