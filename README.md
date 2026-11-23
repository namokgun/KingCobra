King Cobra  — Linked List로 만든 터미널 스네이크 게임

1. 전체 구조

| 파일 | 역할 | 호출하는 모듈 |
| `Config.h` | 맵 크기·점수·속도 상수, `Position` 구조체 | - |
| `LinkedList.h/.c` | 단순 연결 리스트 | - |
| `Snake.h/.c` | 방향 전환, 이동, 성장, 벽·몸통 충돌 판정 | LinkedList |
| `Food.h/.c` | 뱀과 겹치지 않는 랜덤 먹이 생성 | LinkedList |
| `Screen.h/.c` | 화면 출력, 키 입력, OS 종속 코드(`#ifdef _WIN32`) | Snake, Food |
| `Game.h/.c` | 게임 규칙, 한 프레임 진행, 메인 루프 | Snake, Food, Screen |
| `main.c` | 초기화 -> 게임 반복 -> 메모리 해제 | Game, Screen |

2. 컴파일 및 실행
- 조작: 방향키 또는 W A S D / 종료 Q / 게임오버 후 재시작 R
- Windows는 Windows Terminal 또는 PowerShell에서 실행
