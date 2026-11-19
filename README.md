# King Cobra  — Linked List로 만든 터미널 스네이크 게임

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
