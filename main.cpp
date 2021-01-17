#include <iostream>

#ifdef COLORS
#include <ncurses.h>
#else

#include "./game/ncursim.h"

#endif

int main() {
    initscr();
    printw("Hellow world!!");
    getch();
    endwin();
    return 0;
}