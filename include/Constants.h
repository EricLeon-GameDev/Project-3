#pragma once
#include <Arduino.h>
#include <M5Core2.h>

constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 240;

constexpr uint16_t COLOR_BG    = BLACK;
constexpr uint16_t COLOR_BALL  = WHITE;
constexpr uint16_t COLOR_NET   = DARKGREY;
constexpr uint16_t COLOR_GOAL  = GREEN;
constexpr uint16_t COLOR_LEFT  = RED;
constexpr uint16_t COLOR_RIGHT = BLUE;
constexpr uint16_t COLOR_TEXT  = WHITE;

constexpr int GOAL_LINE_LEFT_X  = 8;
constexpr int GOAL_LINE_RIGHT_X = SCREEN_W - 8;

constexpr int PADDLE_W = 8;
constexpr int PADDLE_H = 40;
constexpr int PADDLE_SPEED = 4;

constexpr int BALL_RADIUS = 4;
constexpr int BALL_BASE_SPEED_X = 3;
constexpr int BALL_BASE_SPEED_Y = 2;
constexpr float ABILITY_SPEED_MULT = 1.4f;

constexpr uint8_t WIN_SCORE = 3;

constexpr unsigned long POINT_DELAY_MS = 1000;
constexpr unsigned long ABILITY_WINDOW_MS = 250;
constexpr unsigned long ABILITY_COOLDOWN_MS = 1000;

constexpr unsigned long HOST_STATE_SEND_MS = 33;   // ~30 FPS
constexpr unsigned long CLIENT_INPUT_SEND_MS = 33; // ~30 FPS

enum GameScreenState : uint8_t {
    MENU = 0,
    WAITING_FOR_CONNECTION,
    WAITING_FOR_READY,
    PLAYING,
    POINT_SCORED,
    GAME_OVER
};

enum DeviceRole : uint8_t {
    ROLE_HOST = 0,
    ROLE_CLIENT = 1
};