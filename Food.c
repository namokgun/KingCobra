/* 랜덤 칸 먹이 생성 */
#include <stdlib.h>
#include "Food.h"

 /* 랜덤 좌표를 뽑고, 뱀 몸통과 겹치면 다시 뽑는다. */
int spawnFood(Food* food, linkedList* snakeBody) {
    Position candidate;

    if (getLength(snakeBody) >= MAP_WIDTH * MAP_HEIGHT) {
        return FALSE;                                   /* 맵이 가득 참 (게임 클리어) */
    }

    do {
        candidate.x = rand() % MAP_WIDTH;
        candidate.y = rand() % MAP_HEIGHT;
    } while (searchNode(snakeBody, candidate.x, candidate.y) != NULL);   /* [Linked List] 몸통과 겹치는지 검사 */

    food->pos = candidate;
    return TRUE;
}
