#pragma once
#include "Entities.h"

class Game {
public:
    void begin();
    void resetForNewMatch();
    void updateHost(const LocalInput& hostInput, const ClientInputPacket& clientInput, bool clientConnected);

    GameStatePacket getStatePacket(bool connected) const;

private:
    Paddle leftPaddle{};
    Paddle rightPaddle{};
    Ball ball{};
    Scoreboard score{};

    GameScreenState screenState = MENU;
    unsigned long pointStateStartMs = 0;
    uint8_t winner = 0;

    bool hostReady = false;
    bool clientReady = false;

    void resetBallTowardRight();
    void resetBallTowardLeft();
    void updatePaddle(Paddle& paddle, int16_t delta);
    void handleAbility(Paddle& paddle, bool pressed, unsigned long now);
    void updateBall();
    void checkWallCollision();
    void checkPaddleCollision();
    void checkScoring();
    void enterPointScored();
    void tryLeavePointScored();
};
