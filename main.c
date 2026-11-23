#include <stdlib.h>
#include <time.h>
#include "Game.h"
#include "Screen.h"

int main(void) {
    Game game;
    int wantsRestart;

    srand((unsigned int)time(NULL));   /* 먹이 위치가 매번 달라지도록 */
    openScreen();

    do {
        initGame(&game);
        wantsRestart = playGame(&game);
        destroyGame(&game);            /* 한 판이 끝날 때마다 뱀 메모리 해제 */
    } while (wantsRestart);

    closeScreen();
    return 0;
}
