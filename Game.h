/* 점수, 속도 등 진행상황 상태 선언*/
#ifndef GAME_H
#define GAME_H

#include "Snake.h"
#include "Food.h"

typedef enum { STATE_PLAYING, STATE_GAME_OVER, STATE_WIN } GameState;

typedef struct {
    Snake snake;
    Food food;
    int score;
    int frameDelayMs;    /* 한 칸 이동하는 시간. 작을수록 빠르다. */
    GameState state;
} Game;

void initGame(Game* game);       /* 새 게임 준비 (뱀, 먹이, 점수 초기화) */
void updateGame(Game* game);     /* 한 칸 진행: 충돌 판정 -> 이동 또는 먹이 먹기 */
int  playGame(Game* game);       /* 한 판을 진행하고, 다시 하고 싶으면 TRUE */
void destroyGame(Game* game);    /* 게임이 쓰던 메모리 해제 */

#endif
