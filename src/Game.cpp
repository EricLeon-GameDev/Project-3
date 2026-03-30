#include "Game.h"
#include "Constants.h"
#include <math.h>

void Game::begin() {
    resetForNewMatch();
    screenState = MENU;
}

void Game::resetForNewMatch() {
    leftPaddle = {20, SCREEN_H / 2 - PADDLE_H / 2, PADDLE_W, PADDLE_H, PADDLE_SPEED, COLOR_LEFT, false, 0, 0};
    rightPaddle = {SCREEN_W - 20 - PADDLE_W, SCREEN_H / 2 - PADDLE_H / 2, PADDLE_W, PADDLE_H, PADDLE_SPEED, COLOR_RIGHT, false, 0, 0};
    ball = {SCREEN_W / 2, SCREEN_H / 2, BALL_RADIUS, BALL_BASE_SPEED_X, BALL_BASE_SPEED_Y, COLOR_BALL};
    score = {0, 0};
    winner = 0;
    hostReady = false;
    clientReady = false;
}

void Game::resetBallTowardRight() {
    ball.x = SCREEN_W / 2;
    ball.y = SCREEN_H / 2;
    ball.vx = abs(BALL_BASE_SPEED_X);
    ball.vy = BALL_BASE_SPEED_Y;
}

void Game::resetBallTowardLeft() {
    ball.x = SCREEN_W / 2;
    ball.y = SCREEN_H / 2;
    ball.vx = -abs(BALL_BASE_SPEED_X);
    ball.vy = BALL_BASE_SPEED_Y;
}

void Game::updatePaddle(Paddle& paddle, int16_t delta) {
    paddle.y += delta * paddle.speed;
    if (paddle.y < 0) paddle.y = 0;
    if (paddle.y > SCREEN_H - paddle.height) paddle.y = SCREEN_H - paddle.height;
}

void Game::handleAbility(Paddle& paddle, bool pressed, unsigned long now) {
    if (!pressed) return;
    if (now - paddle.lastAbilityUseTime < ABILITY_COOLDOWN_MS) return;

    paddle.abilityArmed = true;
    paddle.abilityArmTime = now;
    paddle.lastAbilityUseTime = now;
}

void Game::updateBall() {
    ball.x += ball.vx;
    ball.y += ball.vy;
}

void Game::checkWallCollision() {
    if (ball.y - ball.radius <= 0 || ball.y + ball.radius >= SCREEN_H) {
        ball.vy = -ball.vy;
    }
}

void Game::checkPaddleCollision() {
    auto collidePaddle = [&](Paddle& p, bool isLeft) {
        int ballLeft = ball.x - ball.radius;
        int ballRight = ball.x + ball.radius;
        int ballTop = ball.y - ball.radius;
        int ballBottom = ball.y + ball.radius;

        int paddleLeft = p.x;
        int paddleRight = p.x + p.width;
        int paddleTop = p.y;
        int paddleBottom = p.y + p.height;

        bool overlap = !(ballRight < paddleLeft || ballLeft > paddleRight ||
                         ballBottom < paddleTop || ballTop > paddleBottom);

        if (!overlap) return;

        ball.vx = isLeft ? abs(ball.vx) : -abs(ball.vx);

        int paddleCenter = p.y + p.height / 2;
        int offset = ball.y - paddleCenter;
        ball.vy = offset / 6;

        if (ball.vy == 0) {
            ball.vy = (random(0, 2) == 0) ? -1 : 1;
        }

        unsigned long now = millis();
        if (p.abilityArmed && now - p.abilityArmTime <= ABILITY_WINDOW_MS) {
            ball.vx = (int16_t)round(ball.vx * ABILITY_SPEED_MULT);
            if (ball.vx == 0) ball.vx = isLeft ? 1 : -1;
            if (abs(ball.vx) < 4) ball.vx = (ball.vx > 0) ? 4 : -4;
        }

        p.abilityArmed = false;
    };

    collidePaddle(leftPaddle, true);
    collidePaddle(rightPaddle, false);

    unsigned long now = millis();
    if (leftPaddle.abilityArmed && now - leftPaddle.abilityArmTime > ABILITY_WINDOW_MS) {
        leftPaddle.abilityArmed = false;
    }
    if (rightPaddle.abilityArmed && now - rightPaddle.abilityArmTime > ABILITY_WINDOW_MS) {
        rightPaddle.abilityArmed = false;
    }
}

void Game::enterPointScored() {
    screenState = POINT_SCORED;
    pointStateStartMs = millis();
}

void Game::tryLeavePointScored() {
    if (millis() - pointStateStartMs < POINT_DELAY_MS) return;

    if (score.leftScore >= WIN_SCORE) {
        winner = 1;
        screenState = GAME_OVER;
        return;
    }
    if (score.rightScore >= WIN_SCORE) {
        winner = 2;
        screenState = GAME_OVER;
        return;
    }

    if (score.leftScore > score.rightScore) {
        resetBallTowardRight();
    } else {
        resetBallTowardLeft();
    }

    screenState = PLAYING;
}

void Game::checkScoring() {
    if (ball.x + ball.radius < GOAL_LINE_LEFT_X) {
        score.rightScore++;
        resetBallTowardLeft();
        enterPointScored();
        return;
    }

    if (ball.x - ball.radius > GOAL_LINE_RIGHT_X) {
        score.leftScore++;
        resetBallTowardRight();
        enterPointScored();
        return;
    }
}

void Game::updateHost(const LocalInput& hostInput, const ClientInputPacket& clientInput, bool clientConnected) {
    unsigned long now = millis();

    switch (screenState) {
        case MENU:
            if (hostInput.menuSelectPressed || hostInput.touchPressed) {
                screenState = WAITING_FOR_CONNECTION;
            }
            break;

        case WAITING_FOR_CONNECTION:
            if (clientConnected) {
                hostReady = false;
                clientReady = false;
                screenState = WAITING_FOR_READY;
            }
            break;

        case WAITING_FOR_READY:
            if (hostInput.readyPressed || hostInput.touchPressed) hostReady = true;
            if (clientInput.readyPressed || clientInput.touchPressed) clientReady = true;

            if (hostReady && clientReady) {
                score.leftScore = 0;
                score.rightScore = 0;
                winner = 0;
                leftPaddle.y = SCREEN_H / 2 - PADDLE_H / 2;
                rightPaddle.y = SCREEN_H / 2 - PADDLE_H / 2;
                resetBallTowardRight();
                screenState = PLAYING;
            }
            break;

        case PLAYING:
            updatePaddle(leftPaddle, hostInput.paddleDelta);
            updatePaddle(rightPaddle, clientInput.paddleDelta);

            handleAbility(leftPaddle, hostInput.abilityPressed, now);
            handleAbility(rightPaddle, clientInput.abilityPressed, now);

            updateBall();
            checkWallCollision();
            checkPaddleCollision();
            checkScoring();
            break;

        case POINT_SCORED:
            tryLeavePointScored();
            break;

        case GAME_OVER:
            if (hostInput.menuSelectPressed || hostInput.touchPressed ||
                clientInput.menuSelectPressed || clientInput.touchPressed) {
                resetForNewMatch();
                screenState = MENU;
            }
            break;

        default:
            screenState = MENU;
            break;
    }
}

GameStatePacket Game::getStatePacket(bool connected) const {
    GameStatePacket p{};
    p.screenState = (uint8_t)screenState;
    p.leftPaddleY = leftPaddle.y;
    p.rightPaddleY = rightPaddle.y;
    p.ballX = ball.x;
    p.ballY = ball.y;
    p.ballVX = ball.vx;
    p.ballVY = ball.vy;
    p.leftScore = score.leftScore;
    p.rightScore = score.rightScore;
    p.leftAbilityArmed = leftPaddle.abilityArmed;
    p.rightAbilityArmed = rightPaddle.abilityArmed;
    p.leftReady = hostReady;
    p.rightReady = clientReady;
    p.connected = connected;
    p.gameOver = (screenState == GAME_OVER);
    p.winner = winner;
    return p;
}
