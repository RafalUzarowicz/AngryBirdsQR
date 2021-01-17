#ifndef QRGAME_NCURSIM_H
#define QRGAME_NCURSIM_H

int stdscr;

int getmaxyx(int scr, int row, int col);

int move(int x, int y);

int initscr();

int printw(char *string);

int mvprintw(int x, int y, char *string);

int addch(char c);

int mvaddch(int row, int col, char ch);

int addstr();

int getch();

int endwin();

int noecho();

int echo();

#endif //QRGAME_NCURSIM_H
