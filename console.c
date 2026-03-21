#include "console.h"
#include "player.h"
#include <ncurses.h>
#include <locale.h>
#include <math.h>

int SCREEN_WIDTH;
int SCREEN_HEIGHT;

void initConsole(void) {
    setlocale(LC_ALL, "");
    initscr();
    getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);

    cbreak();
    noecho();
    curs_set(0);
    timeout(0);

    // Make getch() return ERR instantly if no key is pressed
    nodelay(stdscr, TRUE);
    // Allow getch() to return arrow keys as single values (KEY_LEFT etc)
    keypad(stdscr, TRUE);
}

void shutdownConsole(void) {
    // Restore the terminal to its original state before we exit
    // Skipping this leaves the terminal broken after the program closes
    endwin();
}

void drawHUD(void) {
    // Print player stats on the last row of the screen
    // mvprintw(row, col, fmt, ...) is ncurses' version of printf at a position
    mvprintw(SCREEN_HEIGHT - 1, 0,
             "X:%.1f Y:%.1f Angle:%.1f  WASD=Move  Arrows=Turn  Q=Quit",
             player.x, player.y, player.angle * 180.0 / M_PI);
}