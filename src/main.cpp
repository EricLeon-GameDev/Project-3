#include <Arduino.h>
#include <M5Core2.h>
#include "Constants.h"
#include "PongServer.h"
#include "PongClient.h"

#if DEVICE_ROLE == ROLE_HOST
PongServer app;
#else
PongClient app;
#endif

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    Serial.begin(115200);
    delay(500);

    app.begin();
}

void loop() {
    app.update();
    delay(5);
}
