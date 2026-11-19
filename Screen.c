/*
 * Screen.c
 * 화면 그리기와 키 입력 처리를 담당합니다.
 * 운영체제 종속 코드(_WIN32 / POSIX)는 이 파일 안에서만 분기합니다.
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L   /* clock_gettime, nanosleep 사용을 위해 필요 */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "Screen.h"

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <time.h>
#endif

/* ---------- 상수 ---------- */
#define ESC_CODE            27
#define POLL_INTERVAL_MS    10
#define OUTPUT_BUFFER_SIZE  32768
#define CELL_WIDTH          2      /* 터미널 글자는 세로로 길어서, 한 칸을 가로 2글자로 그린다 */

#define ANSI_CLEAR_SCREEN   "\033[2J"
#define ANSI_CURSOR_HOME    "\033[H"
#define ANSI_HIDE_CURSOR    "\033[?25l"
#define ANSI_SHOW_CURSOR    "\033[?25h"
#define ANSI_RESET          "\033[0m"
#define ANSI_GREEN          "\033[32m"
#define ANSI_BRIGHT_GREEN   "\033[1;92m"
#define ANSI_RED            "\033[31m"

typedef enum { TILE_EMPTY, TILE_BODY, TILE_HEAD, TILE_FOOD } Tile;

/* ---------- 내부 상태 ---------- */
static volatile sig_atomic_t quitRequested = 0;   /* Ctrl+C가 눌렸는지 */
static int  screenIsOpen = FALSE;
static char outputBuffer[OUTPUT_BUFFER_SIZE];

/* ======================================================================
 *  1부. 운영체제 종속 코드 (터미널 설정 / 키 한 개 읽기 / 시간)
 * ====================================================================== */

/* 입력된 글자(WASD, Q, R)를 명령으로 바꾼다. */
static Command letterToCommand(int ch) {
    switch (ch) {
        case 'w': case 'W': return CMD_UP;
        case 's': case 'S': return CMD_DOWN;
        case 'a': case 'A': return CMD_LEFT;
        case 'd': case 'D': return CMD_RIGHT;
        case 'r': case 'R': return CMD_RESTART;
        case 'q': case 'Q': return CMD_QUIT;
        default:            return CMD_NONE;
    }
}

#ifdef _WIN32

static void setupTerminal(void) {
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(output, &mode)) {
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);   /* ANSI 색/커서 코드 사용 */
    }
}

static void restoreTerminal(void) {}

static long nowMs(void)        { return (long)GetTickCount(); }
static void sleepMs(int ms)    { Sleep((DWORD)ms); }

/* 눌린 키가 있으면 하나 읽어서 명령으로 바꾼다. 없으면 CMD_NONE */
static Command readOneCommand(void) {
    int ch;

    if (!_kbhit()) return CMD_NONE;
    ch = _getch();
    if (ch == 0 || ch == 224) {            /* 방향키는 2바이트로 들어온다 */
        switch (_getch()) {
            case 72: return CMD_UP;
            case 80: return CMD_DOWN;
            case 75: return CMD_LEFT;
            case 77: return CMD_RIGHT;
            default: return CMD_NONE;
        }
    }
    return letterToCommand(ch);
}

#else  /* macOS / Linux */

static struct termios originalTerminal;
static int terminalSaved = FALSE;

/* 엔터 없이 키를 바로 읽도록, 입력 모드를 바꾼다. */
static void setupTerminal(void) {
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &originalTerminal) != 0) return;   /* 터미널이 아니면 건너뜀 */
    terminalSaved = TRUE;
    raw = originalTerminal;
    raw.c_lflag &= ~(ICANON | ECHO);      /* 줄 단위 입력 해제, 입력한 글자 화면 표시 해제 */
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

static void restoreTerminal(void) {
    if (terminalSaved) tcsetattr(STDIN_FILENO, TCSANOW, &originalTerminal);
}

static long nowMs(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

static void sleepMs(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

/* 읽을 키가 대기 중인지 기다리지 않고 확인한다. */
static int isKeyPending(void) {
    fd_set fds;
    struct timeval noWait = { 0, 0 };

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &noWait) > 0;
}

/* 방향키는 ESC [ A/B/C/D 3바이트로 들어온다. ESC 뒤의 2바이트를 읽어 해석한다. */
static Command readArrowKey(void) {
    unsigned char seq[2];

    if (!isKeyPending() || read(STDIN_FILENO, &seq[0], 1) <= 0) return CMD_NONE;
    if (!isKeyPending() || read(STDIN_FILENO, &seq[1], 1) <= 0) return CMD_NONE;
    if (seq[0] != '[' && seq[0] != 'O') return CMD_NONE;

    switch (seq[1]) {
        case 'A': return CMD_UP;
        case 'B': return CMD_DOWN;
        case 'C': return CMD_RIGHT;
        case 'D': return CMD_LEFT;
        default:  return CMD_NONE;
    }
}

/* 눌린 키가 있으면 하나 읽어서 명령으로 바꾼다. 없으면 CMD_NONE */
static Command readOneCommand(void) {
    unsigned char ch;
    ssize_t count;

    if (!isKeyPending()) return CMD_NONE;
    count = read(STDIN_FILENO, &ch, 1);
    if (count == 0) return CMD_QUIT;        /* 입력이 끝났다(EOF) */
    if (count < 0) return CMD_NONE;
    if (ch == ESC_CODE) return readArrowKey();
    return letterToCommand(ch);
}

#endif

/* ======================================================================
 *  2부. 운영체제와 상관없는 코드
 * ====================================================================== */

/* Ctrl+C를 받으면 바로 종료하지 않고 표시만 해 둔다. (터미널 복구를 위해) */
static void handleInterrupt(int signalNumber) {
    (void)signalNumber;
    quitRequested = 1;
}

/* 터미널을 게임용으로 설정한다. 프로그램이 끝날 때 자동으로 복구되도록 등록한다. */
void openScreen(void) {
    if (screenIsOpen) return;
    screenIsOpen = TRUE;

    setvbuf(stdout, outputBuffer, _IOFBF, sizeof(outputBuffer));   /* 한 프레임을 한 번에 출력 (깜빡임 방지) */
    setupTerminal();
    signal(SIGINT, handleInterrupt);
    atexit(closeScreen);
    fputs(ANSI_HIDE_CURSOR, stdout);
    clearScreen();
}

/* 커서와 터미널 설정을 원래대로 되돌린다. */
void closeScreen(void) {
    if (!screenIsOpen) return;
    screenIsOpen = FALSE;

    fputs(ANSI_RESET ANSI_SHOW_CURSOR, stdout);
    fflush(stdout);
    restoreTerminal();
}

void clearScreen(void) {
    fputs(ANSI_CLEAR_SCREEN ANSI_CURSOR_HOME, stdout);
    fflush(stdout);
}

/* 맵의 각 칸에 무엇이 있는지 기록한다. */
static void fillGrid(Tile grid[MAP_HEIGHT][MAP_WIDTH], const Snake* snake, const Food* food) {
    listNode* node;

    memset(grid, TILE_EMPTY, sizeof(Tile) * MAP_WIDTH * MAP_HEIGHT);
    grid[food->pos.y][food->pos.x] = TILE_FOOD;

    node = snake->body->head;                        /* [Linked List] 머리부터 꼬리까지 순회 */
    while (node != NULL) {
        grid[node->y][node->x] = TILE_BODY;
        node = node->link;
    }
    grid[snake->body->head->y][snake->body->head->x] = TILE_HEAD;
}

/* 칸 종류에 맞는 출력 문자열(2글자 + 색)을 돌려준다. */
static const char* tileToText(Tile tile) {
    switch (tile) {
        case TILE_BODY: return ANSI_GREEN        "[]" ANSI_RESET;
        case TILE_HEAD: return ANSI_BRIGHT_GREEN "@@" ANSI_RESET;
        case TILE_FOOD: return ANSI_RED          "()" ANSI_RESET;
        default:        return "  ";
    }
}

static void printBorderLine(void) {
    int i;
    for (i = 0; i < MAP_WIDTH * CELL_WIDTH + 2; i++) putchar('#');
    putchar('\n');
}

/* 테두리와 맵 전체를 출력한다. */
static void printGrid(Tile grid[MAP_HEIGHT][MAP_WIDTH]) {
    int x, y;

    printBorderLine();
    for (y = 0; y < MAP_HEIGHT; y++) {
        putchar('#');
        for (x = 0; x < MAP_WIDTH; x++) {
            fputs(tileToText(grid[y][x]), stdout);
        }
        fputs("#\n", stdout);
    }
    printBorderLine();
}

/* 한 프레임을 그린다: 맵 + 점수 + 조작법 */
void drawGame(const Snake* snake, const Food* food, int score) {
    Tile grid[MAP_HEIGHT][MAP_WIDTH];

    fillGrid(grid, snake, food);
    fputs(ANSI_CURSOR_HOME, stdout);
    printGrid(grid);
    printf("Score: %-5d Length: %-4d\n", score, getLength(snake->body));
    fputs("Move: Arrow keys / WASD     Quit: Q\n", stdout);
    fflush(stdout);
}

/* 게임오버(또는 클리어) 문구를 맵 아래에 출력한다. */
void showResult(int score, int isWin) {
    printf("\n%s  Final score: %d\n", isWin ? "YOU WIN!" : "GAME OVER", score);
    fputs("[R] Restart    [Q] Quit\n", stdout);
    fflush(stdout);
}

/* 지정한 시간 동안 키 입력을 모으고, 마지막으로 눌린 명령을 돌려준다. */
Command readCommandDuring(int milliseconds) {
    Command lastCommand = CMD_NONE;
    long deadline = nowMs() + milliseconds;

    while (nowMs() < deadline) {
        Command command = readOneCommand();
        if (command == CMD_QUIT || quitRequested) return CMD_QUIT;
        if (command != CMD_NONE) lastCommand = command;
        else sleepMs(POLL_INTERVAL_MS);
    }
    return lastCommand;
}

/* R 또는 Q가 눌릴 때까지 기다린다. R이면 TRUE, Q이면 FALSE */
int waitRestartOrQuit(void) {
    while (TRUE) {
        Command command = readCommandDuring(100);
        if (command == CMD_RESTART) return TRUE;
        if (command == CMD_QUIT) return FALSE;
    }
}
