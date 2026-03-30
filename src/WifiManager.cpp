#include "WifiManager.h"
#include "Constants.h"

// ============================================================
// PUT YOUR WIFI INFO HERE
// Replace the text below with your own WiFi network name and password.
// Example:
// const char* WIFI_SSID = "MyHomeWiFi";
// const char* WIFI_PASSWORD = "MyPassword123";
// ============================================================
static const char* WIFI_SSID = "spice8";
static const char* WIFI_PASSWORD = "Mammoth8";

void WifiManager::begin() {
    WiFi.mode(WIFI_STA);
    startConnection();
}

void WifiManager::startConnection() {
    if (strlen(WIFI_SSID) == 0 || String(WIFI_SSID) == "spice8") {
        Serial.println("[WIFI] SSID not set yet. Open src/WifiManager.cpp and add your WiFi name/password.");
        started = false;
        return;
    }

    Serial.printf("[WIFI] Connecting to %s...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    started = true;
    lastRetryMs = millis();
}

void WifiManager::update() {
    if (!started) return;

    if (WiFi.status() == WL_CONNECTED) return;

    unsigned long now = millis();
    if (now - lastRetryMs >= WIFI_RETRY_MS) {
        Serial.println("[WIFI] Retry connect");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        lastRetryMs = now;
    }
}

bool WifiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

const char* WifiManager::getDeviceLabel() const {
#if DEVICE_ROLE == ROLE_HOST
    return WIFI_HOST_LABEL;
#else
    return WIFI_CLIENT_LABEL;
#endif
}
