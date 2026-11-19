/*
 * LinkedList.h
 * 단순 연결 리스트(Singly Linked List)의 자료구조와 함수 선언입니다.
 * 이 게임에서는 리스트의 노드 하나가 "뱀 몸통 한 마디"이고,
 * 리스트의 첫 번째 노드(head)가 뱀의 머리입니다.
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define TRUE  1
#define FALSE 0

/* 노드 = 데이터 필드(x, y) + 링크 필드(link) */
typedef struct ListNode {
    int x;                   /* 가로 좌표 */
    int y;                   /* 세로 좌표 */
    struct ListNode* link;   /* 다음 노드의 주소 */
} listNode;

/* 리스트의 시작(head)과 길이를 함께 관리하는 구조체 */
typedef struct LinkedList {
    listNode* head;
    int length;
} linkedList;

linkedList* initList(void);                              /* 공백 리스트 생성 */
int         getLength(linkedList* L);                    /* 노드 개수 반환 */
void        insertFirst(linkedList* L, int x, int y);    /* 맨 앞에 삽입 */
void        insertLast(linkedList* L, int x, int y);     /* 맨 뒤에 삽입 */
int         delete(linkedList* L, listNode* p);          /* p 노드 삭제 (성공 TRUE / 실패 FALSE) */
listNode*   searchNode(linkedList* L, int x, int y);     /* 좌표가 (x, y)인 노드 검색, 없으면 NULL */
listNode*   getTail(linkedList* L);                      /* 마지막 노드 반환, 빈 리스트면 NULL */
void        displayList(linkedList* L);                  /* 디버깅용 전체 출력 */
void        clear(linkedList* L);                        /* 모든 노드 free, length = 0 */
void        destroyList(linkedList* L);                  /* clear 후 리스트 구조체까지 free */

#endif
