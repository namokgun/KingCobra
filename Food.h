/*먹이 함수, 구조체 선언*/
#ifndef FOOD_H
#define FOOD_H

#include "Config.h"
#include "LinkedList.h"

typedef struct {
    Position pos;   /* 먹이의 현재 위치 */
} Food;

/* 뱀 몸통과 겹치지 않는 빈 칸에 먹이를 만든다. 빈 칸이 없으면 FALSE */
int spawnFood(Food* food, linkedList* snakeBody);

#endif
