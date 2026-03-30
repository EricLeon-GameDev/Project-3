#include "Renderer.h"

void Renderer::begin() {
    M5.Lcd.fillScreen(COLOR_BG);
    M5.Lcd.setTextDatum(TC_DATUM);
}

void Renderer::drawCenteredText(const String& text, int y, uint16_t color, int textSize) {
    M5.Lcd.setTextColor(color, COLOR_BG);
    M5.Lcd.setTextSize(textSize);
    M5.Lcd.drawCentreString(text, SCREEN_W / 2, y, 2);
}

void Renderer::drawThreeDigitScore(int x, int y, uint8_t score, uint16_t color) {
    char buf[4];
    snprintf(buf, sizeof(buf), "%03u", score);
    M5.Lcd.setTextColor(color, COLOR_BG);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(x, y);
    M5.Lcd.print(buf);
}

void Renderer::drawField(const GameStatePacket& state) {
    M5.Lcd.fillScreen(COLOR_BG);

    // Net
    for (int y = 0; y < SCREEN_H; y += 12) {
        M5.Lcd.fillRect(SCREEN_W / 2 - 1, y, 2, 6, COLOR_NET);
    }

    // Goal lines
    M5.Lcd.drawFastVLine(GOAL_LINE_LEFT_X, 0, SCREEN_H, COLOR_GOAL);
    M5.Lcd.drawFastVLine(GOAL_LINE_RIGHT_X, 0, SCREEN_H, COLOR_GOAL);

    // Scores
    drawThreeDigitScore(20, 10, state.leftScore, COLOR_LEFT);
    drawThreeDigitScore(SCREEN_W - 60, 10, state.rightScore, COLOR_RIGHT);

    // Paddles
    M5.Lcd.fillRect(20, state.leftPaddleY, PADDLE_W, PADDLE_H, COLOR_LEFT);
    M5.Lcd.fillRect(SCREEN_W - 20 - PADDLE_W, state.rightPaddleY, PADDLE_W, PADDLE_H, COLOR_RIGHT);

    // Ability indicators
    if (state.leftAbilityArmed) {
        M5.Lcd.drawRect(18, state.leftPaddleY - 2, PADDLE_W + 4, PADDLE_H + 4, YELLOW);
    }
    if (state.rightAbilityArmed) {
        M5.Lcd.drawRect(SCREEN_W - 22 - PADDLE_W, state.rightPaddleY - 2, PADDLE_W + 4, PADDLE_H + 4, YELLOW);
    }

    // Ball
    M5.Lcd.fillCircle(state.ballX, state.ballY, BALL_RADIUS, COLOR_BALL);
}

void Renderer::drawMenu() {
    M5.Lcd.fillScreen(COLOR_BG);
    drawCenteredText("PONG", 40, WHITE, 3);
    drawCenteredText("Touch or BtnC", 100, WHITE, 2);
    drawCenteredText("to Start", 130, WHITE, 2);
}

void Renderer::drawWaitingForConnection(bool isHost, bool connected) {
    M5.Lcd.fillScreen(COLOR_BG);
    drawCenteredText(isHost ? "HOST MODE" : "CLIENT MODE", 40, WHITE, 2);
    drawCenteredText(connected ? "Connected" : "Waiting for player...", 100, connected ? GREEN : WHITE, 2);
}

void Renderer::drawWaitingForReady(bool localReady, bool remoteReady) {
    M5.Lcd.fillScreen(COLOR_BG);
    drawCenteredText("Connected", 40, GREEN, 2);
    drawCenteredText("Touch or BtnC to READY", 90, WHITE, 2);
    drawCenteredText(String("You: ") + (localReady ? "READY" : "NOT READY"), 140, localReady ? GREEN : RED, 2);
    drawCenteredText(String("Other: ") + (remoteReady ? "READY" : "NOT READY"), 170, remoteReady ? GREEN : RED, 2);
}

void Renderer::drawPointScored(uint8_t leftScore, uint8_t rightScore) {
    M5.Lcd.fillScreen(COLOR_BG);
    drawCenteredText("POINT SCORED", 60, WHITE, 2);

    char buf[16];
    snprintf(buf, sizeof(buf), "%03u - %03u", leftScore, rightScore);
    drawCenteredText(buf, 120, WHITE, 3);
}

void Renderer::drawGameOver(uint8_t winner, bool localIsLeftPlayer) {
    M5.Lcd.fillScreen(COLOR_BG);

    bool localWon = (winner == 1 && localIsLeftPlayer) || (winner == 2 && !localIsLeftPlayer);
    uint16_t dotColor = (winner == 1) ? COLOR_LEFT : COLOR_RIGHT;

    M5.Lcd.fillCircle(SCREEN_W / 2, 70, 18, dotColor);
    drawCenteredText(localWon ? "You Win" : "You Lose", 120, WHITE, 3);
    drawCenteredText(winner == 1 ? "Player 1 Wins" : "Player 2 Wins", 165, WHITE, 2);
    drawCenteredText("Touch or BtnC", 200, WHITE, 2);
}

void Renderer::draw(const GameStatePacket& state, bool isHost, bool isConnected) {
    switch ((GameScreenState)state.screenState) {
        case MENU:
            drawMenu();
            break;
        case WAITING_FOR_CONNECTION:
            drawWaitingForConnection(isHost, isConnected);
            break;
        case WAITING_FOR_READY:
            drawField(state);
            drawCenteredText("Waiting for Ready", 210, WHITE, 2);
            break;
        case PLAYING:
            drawField(state);
            break;
        case POINT_SCORED:
            drawPointScored(state.leftScore, state.rightScore);
            break;
        case GAME_OVER:
            drawGameOver(state.winner, isHost); // host = left player
            break;
        default:
            drawMenu();
            break;
    }
}