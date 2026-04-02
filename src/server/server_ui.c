#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include <ncurses.h>
#include <math.h>

#include "server_ui.h"

static char server_log_lines[SERVER_LOG_LINES][SERVER_LOG_LINE_LENGTH];
static int  server_log_head = -1;
static int  server_log_count = 0;

void init_server_ui(void)
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_WHITE, -1);
        init_pair(2, COLOR_CYAN, -1);
        init_pair(3, COLOR_YELLOW, -1);
        init_pair(4, COLOR_RED, -1);
    }

    server_log_head = -1;
    server_log_count = 0;
    server_log("ready");
}

void shutdown_server_ui(void)
{
    endwin();
}

void server_log(const char *fmt, ...)
{
    char line[SERVER_LOG_LINE_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsnprintf(line, sizeof(line), fmt, args);
    va_end(args);

    server_log_head = (server_log_head + 1) % SERVER_LOG_LINES;
    snprintf(server_log_lines[server_log_head], SERVER_LOG_LINE_LENGTH, "%s", line);
    server_log_lines[server_log_head][SERVER_LOG_LINE_LENGTH - 1] = '\0';
    if (server_log_count < SERVER_LOG_LINES)
        server_log_count++;
}

static char entity_dir_char(const Entity *e)
{
    double a = fmod(e->angle, 2 * M_PI);
    if (a < 0) a += 2 * M_PI;
    if (a < M_PI/4 || a >= 7*M_PI/4) return '>';
    if (a < 3*M_PI/4) return 'V';
    if (a < 5*M_PI/4) return '<';
    return '^';
}

void draw_server_ui(Client *clients, nfds_t nfds)
{
    int rows;
    int cols;
    getmaxyx(stdscr, rows, cols);
    erase();

    mvprintw(0, 0, "SERVER MAP  clients=%d  press Q to quit",
             (int)(nfds > 0 ? nfds - 1 : 0));

    int map_left = 2;
    int map_top = 2;
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            char ch = map_solid(x, y) ? '#' : '.';
            mvaddch(map_top + y, map_left + x, ch);
        }
    }

    for (nfds_t i = 1; i < nfds; i++) {
        Entity *e = &clients[i].entity;
        int ex = (int)e->x;
        int ey = (int)e->y;
        if (ex < 0 || ex >= MAP_W || ey < 0 || ey >= MAP_H) continue;

        if (has_colors()) attron(COLOR_PAIR(2) | A_BOLD);
        mvaddch(map_top + ey, map_left + ex, entity_dir_char(e));
        if (has_colors()) attroff(COLOR_PAIR(2) | A_BOLD);
    }

    int info_x = map_left + MAP_W + 4;
    int info_y = map_top;
    mvprintw(info_y++, info_x, "IDX  ID   X    Y   HP");
    for (nfds_t i = 1; i < nfds && info_y < rows - 1; i++) {
        Entity *e = &clients[i].entity;
        mvprintw(info_y++, info_x, "%3d %4d %4.1f %4.1f %3d",
                 (int)i, e->id, e->x, e->y, e->health);
    }

    if (info_y + 2 < rows && info_x < cols) {
        mvprintw(info_y + 1, info_x,
                 "Connected sockets: %d", (int)nfds - 1);
    }

    int log_rows = server_log_count;
    if (log_rows > rows - 3)
        log_rows = rows - 3;

    int log_start = rows - log_rows - 1;
    for (int i = 0; i < log_rows; i++) {
        int idx = (server_log_head - (log_rows - 1 - i) + SERVER_LOG_LINES) % SERVER_LOG_LINES;
        mvprintw(log_start + i, 2, "LOG[%2d]: %.200s", i + 1, server_log_lines[idx]);
    }

    wnoutrefresh(stdscr);
    doupdate();
}

int server_ui_get_input(void)
{
    return getch();
}
