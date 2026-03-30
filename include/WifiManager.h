#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
public:
    void begin();
    void update();
    bool isConnected() const;
    const char* getDeviceLabel() const;

private:
    unsigned long lastRetryMs = 0;
    bool started = false;

    void startConnection();
};
