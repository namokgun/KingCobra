/* 화면 출력, 키 입력 담당 */
#ifndef SCREEN_H
#define SCREEN_H

#include "Snake.h"
#include "Food.h"

/* 키 입력을 게임 명령으로 바꾼 값 */
typedef enum {
    CMD_NONE,
    CMD_UP,
    CMD_DOWN,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_RESTART,
    CMD_QUIT
} Command;

void    openScreen(void);                              /* 터미널을 게임용으로 설정 */
void    closeScreen(void);                             /* 터미널을 원래 상태로 복구 */
void    clearScreen(void);                             /* 화면 전체 지우기 */
void    drawGame(const Snake* snake, const Food* food, int score);   /* 맵 + 점수 출력 */
void    showResult(int score, int isWin);              /* 게임오버/클리어 문구 출력 */
Command readCommandDuring(int milliseconds);           /* 지정한 시간 동안 키를 읽고 마지막 명령 반환 */
int     waitRestartOrQuit(void);                       /* R이면 TRUE, Q이면 FALSE */

#endif
