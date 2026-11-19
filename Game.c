/*
 * Game.c
 * 게임의 규칙과 메인 루프를 담당합니다.
 * 한 프레임의 흐름: 그리기 → 키 입력 → 방향 적용 → 한 칸 진행(updateGame)
 */
#include "Game.h"
#include "Screen.h"

/* 키 명령을 뱀의 방향으로 바꾼다. 방향 명령이 아니면 FALSE */
static int commandToDirection(Command command, Direction* direction) {
    switch (command) {
        case CMD_UP:    *direction = UP;    return TRUE;
        case CMD_DOWN:  *direction = DOWN;  return TRUE;
        case CMD_LEFT:  *direction = LEFT;  return TRUE;
        case CMD_RIGHT: *direction = RIGHT; return TRUE;
        default:        return FALSE;
    }
}

/* 방향 명령이면 뱀의 방향을 바꾼다. */
static void applyCommand(Game* game, Command command) {
    Direction direction;

    if (commandToDirection(command, &direction)) {
        changeDirection(&game->snake, direction);
    }
}

/* 먹이를 먹을 때마다 조금씩 빨라진다. (최소 간격 MIN_FRAME_DELAY_MS) */
static void speedUp(Game* game) {
    if (game->frameDelayMs > MIN_FRAME_DELAY_MS) {
        game->frameDelayMs -= SPEED_UP_STEP_MS;
    }
}

/* 먹이를 먹었을 때: 길이 +1, 점수 +10, 속도 증가, 새 먹이 생성 */
static void eatFood(Game* game, Position next) {
    growSnake(&game->snake, next);            /* [Linked List] 꼬리를 지우지 않으므로 길이가 늘어난다 */
    game->score += SCORE_PER_FOOD;
    speedUp(game);

    if (!spawnFood(&game->food, game->snake.body)) {
        game->state = STATE_WIN;              /* 빈 칸이 없으면 클리어 */
    }
}

/* 새 게임을 준비한다. */
void initGame(Game* game) {
    initSnake(&game->snake);
    spawnFood(&game->food, game->snake.body);
    game->score = 0;
    game->frameDelayMs = START_FRAME_DELAY_MS;
    game->state = STATE_PLAYING;
}

/* 뱀을 한 칸 진행시킨다. */
void updateGame(Game* game) {
    Position next = getNextHeadPosition(&game->snake);
    int willEat = (next.x == game->food.pos.x && next.y == game->food.pos.y);

    if (isWallCollision(next) || isSelfCollision(&game->snake, next, willEat)) {
        game->state = STATE_GAME_OVER;
        return;
    }

    if (willEat) eatFood(game, next);
    else         moveSnake(&game->snake, next);
}

/* 한 판을 진행한다. 끝난 뒤 R을 누르면 TRUE, Q를 누르면 FALSE */
int playGame(Game* game) {
    clearScreen();

    while (game->state == STATE_PLAYING) {
        drawGame(&game->snake, &game->food, game->score);
        Command command = readCommandDuring(game->frameDelayMs);
        if (command == CMD_QUIT) return FALSE;

        applyCommand(game, command);
        updateGame(game);
    }

    drawGame(&game->snake, &game->food, game->score);
    showResult(game->score, game->state == STATE_WIN);
    return waitRestartOrQuit();
}

/* 게임이 쓰던 메모리(뱀 몸통 리스트)를 해제한다. */
void destroyGame(Game* game) {
    destroySnake(&game->snake);
}
