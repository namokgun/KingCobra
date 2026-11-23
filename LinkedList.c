#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"

/* 메모리를 할당하고, 실패하면 메시지를 출력한 뒤 프로그램을 종료한다. */
static void* allocateOrExit(size_t size) {
    void* memory = malloc(size);
    if (memory == NULL) {
        fprintf(stderr, "메모리 할당에 실패했습니다.\n");
        exit(EXIT_FAILURE);
    }
    return memory;
}

/* 새 노드를 하나 만든다. (데이터 저장 + 링크는 NULL로 초기화) */
static listNode* createNode(int x, int y) {
    listNode* node = (listNode*)allocateOrExit(sizeof(listNode));
    node->x = x;
    node->y = y;
    node->link = NULL;
    return node;
}

/* 공백 리스트를 만든다. */
linkedList* initList(void) {
    linkedList* L = (linkedList*)allocateOrExit(sizeof(linkedList));
    L->head = NULL;
    L->length = 0;
    return L;
}

/* 리스트의 길이를 반환 */
int getLength(linkedList* L) {
    return L->length;
}

/* 리스트의 첫 번째 노드로 삽입 (뱀의 새 머리) */
void insertFirst(linkedList* L, int x, int y) {
    listNode* newNode = createNode(x, y);   /* 1. 노드 준비 + 2. 데이터 저장 */
    newNode->link = L->head;                /* 3. 새 노드가 기존 첫 노드를 가리키게 함 */
    L->head = newNode;                      /* 4. head가 새 노드를 가리키게 함 */
    L->length++;
}

/* 리스트의 마지막 노드로 삽입 */
void insertLast(linkedList* L, int x, int y) {
    listNode* newNode = createNode(x, y);   /* 새 노드의 link는 NULL */

    if (L->head == NULL) {                  /* 공백 리스트에 삽입 */
        L->head = newNode;
    }
    else {
        getTail(L)->link = newNode;         /* 현재 마지막 노드 뒤에 연결 */
    }
    L->length++;
}

/* 마지막 노드를 찾아 반환 */
listNode* getTail(linkedList* L) {
    listNode* temp = L->head;

    if (temp == NULL) return NULL;
    while (temp->link != NULL) {
        temp = temp->link;
    }
    return temp;
}

/* p 노드를 리스트에서 삭제. 성공하면 TRUE, 실패하면 FALSE */
int delete(linkedList* L, listNode* p) {
    listNode* pre;

    if (L == NULL || L->head == NULL) return FALSE;   /* 공백 리스트 */
    if (p == NULL) return FALSE;                      /* 삭제할 노드가 없음 */

    if (L->head == p) {                               /* 첫 번째 노드 삭제 */
        L->head = p->link;
    }
    else {
        pre = L->head;                                /* 1. 삭제할 노드의 앞 노드 찾기 */
        while (pre != NULL && pre->link != p) {
            pre = pre->link;
        }
        if (pre == NULL) return FALSE;                /* p가 리스트에 없음 */
        pre->link = p->link;                          /* 2. 앞 노드가 p의 다음 노드를 가리키게 함 */
    }
    free(p);                                          /* 3. 삭제한 노드의 메모리 해제 */
    L->length--;
    return TRUE;
}

/* 좌표가 (x, y)인 노드를 처음부터 차례로 찾는다. 없으면 NULL */
listNode* searchNode(linkedList* L, int x, int y) {
    listNode* temp = L->head;

    while (temp != NULL) {
        if (temp->x == x && temp->y == y) return temp;
        temp = temp->link;
    }
    return NULL;
}

/* 리스트의 모든 노드를 출력 (디버깅용) */
void displayList(linkedList* L) {
    listNode* p = L->head;

    printf("L=( ");
    while (p != NULL) {
        printf("(%d,%d)", p->x, p->y);
        if (p->link != NULL) printf(" --> ");
        p = p->link;
    }
    printf(" )  length=%d\n", L->length);
}

/* 리스트의 모든 노드를 삭제 */
void clear(linkedList* L) {
    listNode* p;

    while (L->head != NULL) {
        p = L->head;
        L->head = L->head->link;    /* head를 먼저 옮겨 놓고 이전 첫 노드를 해제*/
        free(p);
    }
    L->length = 0;
}

/* 노드를 모두 지우고, 리스트 구조체까지 해제 */
void destroyList(linkedList* L) {
    if (L == NULL) return;
    clear(L);
    free(L);
}
