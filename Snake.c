/*
 * Snake.c
 * 뱀의 이동, 방향 전환, 성장, 충돌 판정을 담당합니다.
 * Linked List가 쓰이는 핵심 지점에는 "[Linked List]" 주석을 달아 두었습니다.
 */
#include <stddef.h>
#include "Snake.h"

/* 두 방향이 서로 정반대인지 확인한다. (UP-DOWN, LEFT-RIGHT) */
static int isOppositeDirection(Direction a, Direction b) {
    return (a == UP    && b == DOWN)  || (a == DOWN  && b == UP) ||
           (a == LEFT  && b == RIGHT) || (a == RIGHT && b == LEFT);
}

/* 맵 중앙에 길이 INITIAL_SNAKE_LENGTH의 뱀을 만든다. 오른쪽을 향해 시작한다. */
void initSnake(Snake* snake) {
    int startX = MAP_WIDTH / 2;
    int startY = MAP_HEIGHT / 2;
    int i;

    snake->body = initList();           /* [Linked List] 공백 리스트 생성 */
    snake->direction = RIGHT;

    for (i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        insertLast(snake->body, startX - i, startY);   /* [Linked List] 머리 → 꼬리 순서로 연결 */
    }
}

/* 뱀의 몸통 메모리를 모두 해제한다. */
void destroySnake(Snake* snake) {
    destroyList(snake->body);           /* [Linked List] 모든 노드 free */
    snake->body = NULL;
}

/* 방향을 바꾼다. 진행 방향의 정반대로는 바꿀 수 없다. */
void changeDirection(Snake* snake, Direction newDirection) {
    if (isOppositeDirection(snake->direction, newDirection)) return;
    snake->direction = newDirection;
}

/* 현재 방향으로 한 칸 갔을 때 머리의 좌표를 계산한다. */
Position getNextHeadPosition(const Snake* snake) {
    Position next = { snake->body->head->x, snake->body->head->y };

    switch (snake->direction) {
        case UP:    next.y--; break;
        case DOWN:  next.y++; break;
        case LEFT:  next.x--; break;
        case RIGHT: next.x++; break;
    }
    return next;
}

/* 한 칸 이동: 새 머리를 앞에 붙이고, 꼬리를 떼어낸다. (길이는 그대로) */
void moveSnake(Snake* snake, Position next) {
    insertFirst(snake->body, next.x, next.y);      /* [Linked List] 새 머리 노드를 앞에 삽입 */
    delete(snake->body, getTail(snake->body));     /* [Linked List] 꼬리 노드를 삭제 */
}

/* 먹이를 먹고 이동: 새 머리만 붙이고 꼬리는 그대로 둔다. (길이 +1) */
void growSnake(Snake* snake, Position next) {
    insertFirst(snake->body, next.x, next.y);      /* [Linked List] 새 머리 노드만 삽입 */
}

/* 맵 바깥(벽)에 닿았는지 확인한다. */
int isWallCollision(Position pos) {
    return pos.x < 0 || pos.x >= MAP_WIDTH ||
           pos.y < 0 || pos.y >= MAP_HEIGHT;
}

/* 다음 머리 위치에 이미 몸통이 있는지 확인한다. */
int isSelfCollision(const Snake* snake, Position next, int willGrow) {
    listNode* hit = searchNode(snake->body, next.x, next.y);   /* [Linked List] 좌표로 노드 검색 */

    if (hit == NULL) return FALSE;
    /* 성장하지 않으면 이번 이동에서 꼬리가 사라지므로, 꼬리 칸으로 들어가는 것은 충돌이 아니다. */
    if (!willGrow && hit == getTail(snake->body)) return FALSE;
    return TRUE;
}
