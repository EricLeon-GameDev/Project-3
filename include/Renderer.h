#pragma once
#include <M5Core2.h>
#include "Entities.h"
#include "Constants.h"

class Renderer {
public:
    void begin();
    void draw(const GameStatePacket& state, bool isHost, bool isConnected);
    void drawMenu();
    void drawWaitingForConnection(bool isHost, bool connected);
    void drawWaitingForReady(bool localReady, bool remoteReady);
    void drawGameOver(uint8_t winner, bool localIsLeftPlayer);
    void drawPointScored(uint8_t leftScore, uint8_t rightScore);

private:
    void drawCenteredText(const String& text, int y, uint16_t color, int textSize = 2);
    void drawThreeDigitScore(int x, int y, uint8_t score, uint16_t color);
    void drawField(const GameStatePacket& state);
};
