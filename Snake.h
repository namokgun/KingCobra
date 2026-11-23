/* 뱀의 구조체와 이동, 성장, 충돌 판정 함수 선언
뱀 몸통 = LinkedList, 머리 = 리스트의 첫 번째 노드 */

#ifndef SNAKE_H
#define SNAKE_H

#include "Config.h"
#include "LinkedList.h"

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    linkedList* body;        /* 몸통 리스트 (head = 머리) */
    Direction direction;     /* 현재 진행 방향 */
} Snake;

void     initSnake(Snake* snake);                       /* 맵 중앙에 길이 3의 뱀 생성 */
void     destroySnake(Snake* snake);                    /* 몸통 메모리 전체 해제 */
void     changeDirection(Snake* snake, Direction d);    /* 방향 전환 (정반대 방향은 무시) */
Position getNextHeadPosition(const Snake* snake);       /* 이번 이동에서 머리가 도착할 칸 */
void     moveSnake(Snake* snake, Position next);        /* 한 칸 이동: 머리 추가 + 꼬리 삭제 */
void     growSnake(Snake* snake, Position next);        /* 성장하며 이동: 머리만 추가 */
int      isWallCollision(Position pos);                 /* 벽 밖인가? */
int      isSelfCollision(const Snake* snake, Position next, int willGrow);  /* 자기 몸에 부딪히는가? */

#endif
