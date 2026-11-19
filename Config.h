/*
 * Config.h
 * 게임 전체에서 함께 쓰는 상수와 좌표 구조체를 모아둔 파일입니다.
 * 맵 크기, 점수, 속도를 바꾸고 싶으면 이 파일만 수정하면 됩니다.
 */
#ifndef CONFIG_H
#define CONFIG_H

/* 맵 크기 (칸 단위) */
#define MAP_WIDTH   30
#define MAP_HEIGHT  20

/* 뱀 / 점수 */
#define INITIAL_SNAKE_LENGTH  3
#define SCORE_PER_FOOD        10

/* 속도: 한 칸 이동하는 데 걸리는 시간(ms). 먹이를 먹을 때마다 조금씩 빨라진다. */
#define START_FRAME_DELAY_MS  150
#define MIN_FRAME_DELAY_MS    60
#define SPEED_UP_STEP_MS      5

/* 맵 위의 좌표 (왼쪽 위가 (0, 0)) */
typedef struct {
    int x;
    int y;
} Position;

#endif
