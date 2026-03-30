#pragma once
#include <Arduino.h>
#include "Constants.h"

struct Paddle {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t speed;
    uint16_t color;

    bool abilityArmed;
    unsigned long abilityArmTime;
    unsigned long lastAbilityUseTime;
};

struct Ball {
    int16_t x;
    int16_t y;
    int16_t radius;
    int16_t vx;
    int16_t vy;
    uint16_t color;
};

struct Scoreboard {
    uint8_t leftScore;
    uint8_t rightScore;
};

#pragma pack(push, 1)
struct ClientInputPacket {
    int16_t paddleDelta;
    bool abilityPressed;
    bool menuSelectPressed;
    bool readyPressed;
    bool touchPressed;
};

struct GameStatePacket {
    uint8_t screenState;
    int16_t leftPaddleY;
    int16_t rightPaddleY;
    int16_t ballX;
    int16_t ballY;
    int16_t ballVX;
    int16_t ballVY;
    uint8_t leftScore;
    uint8_t rightScore;
    bool leftAbilityArmed;
    bool rightAbilityArmed;
    bool leftReady;
    bool rightReady;
    bool connected;
    bool gameOver;
    uint8_t winner;
};
#pragma pack(pop)

struct LocalInput {
    int16_t paddleDelta = 0;
    bool abilityPressed = false;
    bool menuSelectPressed = false;
    bool readyPressed = false;
    bool touchPressed = false;
};
