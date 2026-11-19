# King Cobra 🐍 — Linked List로 만든 터미널 스네이크 게임

## 1. 전체 구조

| 파일 | 역할 | 호출하는 모듈 |
|---|---|---|
| `Config.h` | 맵 크기·점수·속도 상수, `Position` 구조체 | - |
| `LinkedList.h/.c` | 단순 연결 리스트 (강의자료 ADT). **뱀의 몸통** | - |
| `Snake.h/.c` | 방향 전환, 이동, 성장, 벽·몸통 충돌 판정 | LinkedList |
| `Food.h/.c` | 뱀과 겹치지 않는 랜덤 먹이 생성 | LinkedList |
| `Screen.h/.c` | 화면 출력, 키 입력, OS 종속 코드(`#ifdef _WIN32`) | Snake, Food |
| `Game.h/.c` | 게임 규칙, 한 프레임 진행, 메인 루프 | Snake, Food, Screen |
| `main.c` | 초기화 → 게임 반복 → 메모리 해제 (약 25줄) | Game, Screen |

```
main → Game → Snake → LinkedList
         ├──→ Food  → LinkedList
         └──→ Screen (화면/입력)
```

## 2. 컴파일 및 실행

```bash
# macOS / Linux
make
./kingcobra

# make가 없을 때 (Windows MinGW 포함)
gcc -std=c99 main.c Game.c Snake.c Food.c Screen.c LinkedList.c -o kingcobra
kingcobra.exe     # Windows
```

- 조작: 방향키 또는 W A S D / 종료 Q / 게임오버 후 재시작 R
- Windows는 **Windows Terminal 또는 PowerShell**에서 실행하세요. (색·커서 숨김에 ANSI 코드를 사용)
- Visual Studio(MSVC)로 컴파일하면 `delete` 같은 이름은 문제없지만, 가급적 gcc(MinGW)를 권장합니다.

## 3. 강의자료 대비 달라진 점 (발표 때 설명하면 좋은 부분)

| 강의자료 | 이 프로젝트 |
|---|---|
| 노드 데이터: 상품번호·과일이름·가격 | 노드 데이터: 좌표 `x`, `y` |
| `typedef struct ListNode { ... listNode* link; }` (선언 전에 `listNode` 사용) | `struct ListNode* link;` 로 올바르게 수정 |
| `delete`: `pre`가 NULL이 된 뒤에 `pre->link`를 읽을 수 있음 | `while (pre != NULL && pre->link != p)` 로 NULL 검사를 먼저 수행 |
| `void main()` | `int main(void)` |
| 추가한 함수 | `getTail` (꼬리 찾기), `destroyList` (리스트 구조체까지 해제) |

## 4. 발표용 보조 자료

### 4-1. 뱀이 한 칸 이동할 때 Linked List에서 일어나는 일

예) 오른쪽으로 이동, 뱀 = `(15,10) → (14,10) → (13,10)` (머리 → 꼬리)

1. `getNextHeadPosition`: 머리 좌표에서 방향만큼 더해 **다음 머리 위치 (16,10)** 계산
2. `isWallCollision`, `isSelfCollision`으로 충돌 검사 (`searchNode`로 몸통 검색)
3. `moveSnake` 실행
   - `insertFirst(16,10)` → 새 노드가 기존 head를 가리키고, head가 새 노드를 가리킴 (리스트가 4개가 됨)
   - `delete(getTail())` → 마지막 노드 앞 노드를 찾아 `link = NULL`, 꼬리 `free`
4. 결과: `(16,10) → (15,10) → (14,10)` 길이 3 유지

> 뱀 몸통을 전부 한 칸씩 옮기는 게 아니라 **머리 추가 + 꼬리 삭제 두 번의 연산**만으로 이동이 표현됩니다.

### 4-2. 먹이를 먹었을 때와의 차이

| | 일반 이동 | 먹이 먹기 |
|---|---|---|
| 함수 | `moveSnake` | `growSnake` (`eatFood` 안에서 호출) |
| 머리 | `insertFirst`로 추가 | `insertFirst`로 추가 |
| 꼬리 | `delete`로 삭제 | **삭제하지 않음** |
| 길이 | 그대로 | +1 |
| 추가 처리 | 없음 | 점수 +10, 속도 증가, `spawnFood`로 새 먹이 |

### 4-3. 예상 질문과 답변

**Q1. 왜 배열이 아니라 Linked List를 썼나요?**
뱀은 머리에 한 칸 추가되고 꼬리에서 한 칸 빠지며 길이가 계속 변합니다. 배열은 길이 제한이 있고, 앞에 추가하려면 전체 원소를 한 칸씩 밀어야 합니다(오버헤드). Linked List는 노드를 필요할 때 동적으로 만들고, 앞에 삽입할 때 링크만 바꾸면 됩니다.

**Q2. 꼬리를 삭제할 때 O(n)이라고 하던데, 왜 그런가요? 개선할 수 있나요?**
단순 연결 리스트는 `다음` 방향 링크만 있어서, 꼬리를 지우려면 head부터 따라가며 꼬리의 앞 노드를 찾아야 합니다. `getTail`과 `delete` 둘 다 순회해서 O(n)입니다. 개선하려면 (1) 리스트 구조체에 `tail` 포인터를 추가하거나 (2) 이중 연결 리스트를 사용해 꼬리의 앞 노드를 바로 알 수 있게 하면 O(1)이 됩니다. 이 맵은 최대 600칸이라 체감 성능 차이는 없어서, 강의에서 배운 단순 연결 리스트 그대로 구현했습니다.

**Q3. 메모리 누수는 어떻게 막았나요?**
노드를 만드는 곳은 `createNode`(malloc) 하나로 모으고, 해제하는 곳은 `delete`(꼬리 제거)와 `clear/destroyList`(전체 제거)로만 한정했습니다. 한 판이 끝날 때마다 `destroyGame`으로 뱀 몸통을 해제하고, AddressSanitizer로 100판 반복 테스트에서 누수가 없음을 확인했습니다.

### 4-4. 제안서에서 걱정한 부분 처리

- **메모리 해제**: 위 Q3와 같음. `main`의 반복문에서 `destroyGame`을 호출하고, 비정상 종료 시에도 `atexit`로 터미널이 복구됩니다. Ctrl+C도 플래그로 받아 정상 종료합니다.
- **먹이 생성**: `spawnFood`가 랜덤 좌표를 뽑고 `searchNode`로 뱀 몸통과 겹치면 다시 뽑습니다. 맵이 가득 차면(`length == 맵 칸 수`) 무한 반복 대신 `FALSE`를 반환해 클리어로 처리합니다.
- **꼬리 칸 이동**: 성장하지 않을 때는 이번 이동에서 꼬리가 비워지므로, 꼬리 칸으로 들어가는 것은 충돌로 보지 않습니다. (`isSelfCollision`의 `willGrow` 인자)

## 5. 직접 바꿔 볼 수 있는 값 (`Config.h`)

`MAP_WIDTH`, `MAP_HEIGHT`, `START_FRAME_DELAY_MS`(시작 속도), `SPEED_UP_STEP_MS`(가속량), `SCORE_PER_FOOD`
