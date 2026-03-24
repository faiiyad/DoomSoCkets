#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include "defs.h"
#include "map.h"
#include "player.h"
#include "gun.h"
#include "render.h"
#include "title.h"

static void init_colors(void)
{
    start_color();
    use_default_colors();

    // ── 3D view ───────────────────────────────────────────────────────────
    init_pair(CP_WALL1,    94,  -1);
    init_pair(CP_WALL2,    51,  -1);   // bright cyan 
    init_pair(CP_WALL3,    46,  -1);   // bright green
    init_pair(CP_WALL4,    226, -1);   // bright yellow 
    init_pair(CP_XWALL,    58,  -1);   // olive — darkerface
    init_pair(CP_FLOOR,    240, -1);
    init_pair(CP_CEIL,     17,  -1);   // dark navy
    init_pair(CP_HUD,      COLOR_BLACK, COLOR_WHITE);

    // ── minimap — radar theme ─────────────────────────────────────────────
    init_pair(CP_MAP_BDR,   64,  22);  // bright green on near-black  — border
    // init_pair(CP_MAP_BDR,   46,  232);  // bright green on near-black  — border
    init_pair(CP_MAP_P,    124,  232);  // yellow-green on near-black  — player
    init_pair(CP_MAP_EMPTY, 232,   232);  // dark green on dark green    — solid empty
    init_pair(CP_WALL1_M,   28,   28);  // mid green on mid green      — solid wall
    init_pair(CP_WALL2_M,   34,   34);  // green on green
    init_pair(CP_WALL3_M,   40,   40);  // bright green on bright green
    init_pair(CP_WALL4_M,   82,   82);  // lime on lime

    // ── gun ───────────────────────────────────────────────────────────────
    init_pair(CP_GUN,      238, -1);
    init_pair(CP_FLASH,    226, -1);
    init_pair(HAND_CLR,    124, -1);
    init_pair(HAND_CLR_S,  131, -1);
    init_pair(GUN_BDR,     255, -1);
    init_pair(MUZ_1,       230, -1);
    init_pair(MUZ_2,       217, -1);
    init_pair(MUZ_3,       88,  -1);
    init_pair(GUN_BODY,    64,  -1);
    init_pair(GUN_TRIM,    22,  -1);
    init_pair(GUN_DIRT,    58,  -1);
  
    init_pair(TITLE1, 93, -1);
    init_pair(TITLE2, 90, -1);
    init_pair(TITLE3, 202, -1);
    init_pair(TITLE4, 208, -1);
    init_pair(TITLEBG, 0, -1);
}

int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    init_colors();

    Player p = { 8.0, 8.0, 0.0 };
    map_find_spawn(&p.x, &p.y);

    show_title_screen();
    flushinp(); 

    int show_map = 1;

    struct timespec ts = { 0, 16000000L };  // ~60 fps

    while (1) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        if (ch == 'm' || ch == 'M') show_map = !show_map;

        if (ch == 'k' || ch == 'K') p.angle -= ROT_SPD;
        if (ch == 'l' || ch == 'L') p.angle += ROT_SPD;

        double nx = p.x, ny = p.y;
        if (ch == KEY_UP   || ch == 'w' || ch == 'W') {
            nx += cos(p.angle) * MOVE_SPD;
            ny += sin(p.angle) * MOVE_SPD;
        }
        if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            nx -= cos(p.angle) * MOVE_SPD;
            ny -= sin(p.angle) * MOVE_SPD;
        }
        if (ch == 'a' || ch == 'A' || ch == ',') {
            nx += cos(p.angle - M_PI / 2) * MOVE_SPD;
            ny += sin(p.angle - M_PI / 2) * MOVE_SPD;
        }
        if (ch == 'd' || ch == 'D' || ch == '.') {
            nx += cos(p.angle + M_PI / 2) * MOVE_SPD;
            ny += sin(p.angle + M_PI / 2) * MOVE_SPD;
        }

        if (ch == ' ' && gun_frame == 0) {
            gun_frame = 1;
            gun_timer = 10;
        }
        if (gun_timer > 0) {
            gun_timer--;
            if (gun_timer == 0) gun_frame = 0;
        }
        // double guns
        if (ch == 'e' || ch == 'E'){
            if (gun_status == 0){
                gun_status = 1;
            }
            else{
                gun_status = 0;
            }
        }

        double margin = 0.2;
        if (!map_solid((int)(nx + margin), (int)(p.y)) &&
            !map_solid((int)(nx - margin), (int)(p.y))) p.x = nx;
        if (!map_solid((int)(p.x), (int)(ny + margin)) &&
            !map_solid((int)(p.x), (int)(ny - margin))) p.y = ny;

        render(&p, show_map);
        nanosleep(&ts, NULL);
    }

    endwin();
    printf("YO THANKS FOR PLAYING\n");
    return 0;
}