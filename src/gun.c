#include <math.h>
#include <stdlib.h>
#include <ncurses.h>
#include "gun.h"
#include "defs.h"

int gun_frame = 0;
int gun_timer = 0;

void draw_gun(int rows, int cols)
{
    int kick = (gun_frame == 1) ? -1 : 0;
    int base = rows  + kick;
    int cx   = cols / 2 - 5;
    int r    = base - 20;

    #define BDR   attron(COLOR_PAIR(CP_GUN) | A_BOLD)
    #define BDR_  attroff(COLOR_PAIR(CP_GUN) | A_BOLD)
    #define GRY   attron(COLOR_PAIR(CP_MAP_P) | A_BOLD)
    #define GRY_  attroff(COLOR_PAIR(CP_MAP_P) | A_BOLD)
    #define SCP   attron(COLOR_PAIR(CP_WALL2) | A_BOLD)
    #define SCP_  attroff(COLOR_PAIR(CP_WALL2) | A_BOLD)
    #define DOT   attron(COLOR_PAIR(CP_FLASH) | A_BOLD)
    #define DOT_  attroff(COLOR_PAIR(CP_FLASH) | A_BOLD)
    #define INT1  attron(COLOR_PAIR(HAND_CLR) | A_BOLD)
    #define INT1_ attroff(COLOR_PAIR(HAND_CLR) | A_BOLD)
    #define INT2  attron(COLOR_PAIR(HAND_CLR_S) | A_BOLD)
    #define INT2_ attroff(COLOR_PAIR(HAND_CLR_S) | A_BOLD)
    #define GUNO  attron(COLOR_PAIR(GUN_BDR) | A_BOLD)
    #define GUNO_ attroff(COLOR_PAIR(GUN_BDR) | A_BOLD)

    if (gun_frame == 1) {
        int cy  = r;
        int cx0 = cx + 5;

        for (int dy = -5; dy <= 5; dy++) {
            for (int dx = -8; dx <= 8; dx++) {
                float dist = sqrtf((dx * 0.55f) * (dx * 0.55f) + (float)(dy * dy));

                if (dist < 2.0f) continue;

                char ch;
                int  pair;
                if (dist < 2.0f) {
                    ch = 'O'; pair = MUZ_1;
                } else if (dist < 3.50f) {
                    ch = 'o'; pair = MUZ_1;
                } else if (dist < 4.0f) {
                    ch = '*'; pair = MUZ_2;
                } else if (dist < 4.5f && (rand() % 3 != 0)) {
                    ch = '.'; pair = MUZ_3;
                } else {
                    continue;
                }

                if (rand() % 3 == 0) ch = ' ';
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(cy + dy, cx0 + dx, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }

    /* sight */
    GUNO; mvaddch (r-1, cx+3,  '_'); GUNO_;
    GUNO; mvaddch (r-1, cx+4,  ACS_BLOCK);        GUNO_;
    INT1; mvaddch (r-1, cx+5,   '-'); INT1_;
    GUNO; mvaddch (r-1, cx+6,  ACS_BLOCK);        GUNO_;
    GUNO; mvaddch (r-1, cx+7,  '_'); GUNO_;

    /* top taper — corners rounded */
    GUNO; mvaddch (r,   cx+2,  '/');         GUNO_;
    BDR;  mvprintw(r,   cx+3,  "==");        BDR_;
    BDR;  mvaddch (r,   cx+5,  ACS_BLOCK);  BDR_;
    BDR;  mvprintw(r,   cx+6,  "==");        BDR_;
    GUNO; mvaddch (r,   cx+8,  '\\');        GUNO_;

    GUNO; mvaddch (r+1, cx+1,  '/');         GUNO_;
    BDR;  mvprintw(r+1, cx+2,  "A##");       BDR_;
    BDR;  mvaddch (r+1, cx+5,  ACS_BLOCK);  BDR_;
    BDR;  mvprintw(r+1, cx+6,  "##A");       BDR_;
    GUNO; mvaddch (r+1, cx+9,  '\\');        GUNO_;

    GUNO; mvaddch (r+2, cx,    '/');         GUNO_;
    BDR;  mvprintw(r+2, cx+1,  "A###");      BDR_;
    BDR;  mvaddch (r+2, cx+5,  ACS_BLOCK);  BDR_;
    BDR;  mvprintw(r+2, cx+6,  "###A");      BDR_;
    GUNO; mvaddch (r+2, cx+10, '\\');        GUNO_;

    GUNO; mvaddch (r+3, cx-1,  '|');         GUNO_;
    BDR;  mvprintw(r+3, cx,    "A####");     BDR_;
    BDR;  mvaddch (r+3, cx+5,  ACS_BLOCK);  BDR_;
    BDR;  mvprintw(r+3, cx+6,  "####A");     BDR_;
    GUNO; mvaddch (r+3, cx+11, '|');         GUNO_;

    /* main body */
    GUNO; mvprintw(r+4, cx-1,         "[###########]");   GUNO_;

    GUNO; mvaddch (r+5, cx-1,  '|');        GUNO_;
    BDR;  mvaddch (r+5, cx,    '[');        BDR_;
    INT1; mvaddch (r+5, cx+1,  ACS_BLOCK); INT1_;
    BDR;  mvaddch (r+5, cx+2,  ']');        BDR_;
    BDR;  mvaddch (r+5, cx+3,  '[');        BDR_;
    INT1; mvaddch (r+5, cx+4,  ACS_BLOCK); INT1_;
    INT1; mvaddch (r+5, cx+5,  ACS_BLOCK); INT1_;
    INT1; mvaddch (r+5, cx+6,  ACS_BLOCK); INT1_;
    BDR;  mvaddch (r+5, cx+7,  ']');        BDR_;
    BDR;  mvaddch (r+5, cx+8,  '[');        BDR_;
    INT1; mvaddch (r+5, cx+9,  ACS_BLOCK); INT1_;
    BDR;  mvaddch (r+5, cx+10, ']');        BDR_;
    GUNO; mvaddch (r+5, cx+11, '|');        GUNO_;

    GUNO; mvaddch (r+6, cx-1,  '|');                       GUNO_;
    BDR; mvprintw(r+6, cx,    "###########");              BDR_;
    GUNO; mvaddch (r+6, cx+11, '|');                       GUNO_;

    GUNO; mvaddch (r+7, cx-1,  '|');                       GUNO_;
    BDR;  mvprintw(r+7, cx,    "###########");              BDR_;
    GUNO; mvaddch (r+7, cx+11, '|');                       GUNO_;

    GUNO; mvprintw(r+8, cx,    "\\@@@@@@@@@/");            GUNO_;

    /* grip — sides straight, base same */
    /* grip (5 wide: cx+2 to cx+6) */
        /* grip (5 wide: cx+2 to cx+6) */
    GUNO; mvaddch (r+9,  cx+2, '|');        GUNO_;
    BDR;  mvaddch (r+9,  cx+3, 'H');        BDR_;
    INT1; mvaddch (r+9,  cx+4, '[');        INT1_;
    INT2; mvaddch (r+9,  cx+5, ACS_BLOCK); INT2_;
    INT1; mvaddch (r+9,  cx+6, ']');        INT1_;
    BDR;  mvaddch (r+9,  cx+7, 'H');        BDR_;
    GUNO; mvaddch (r+9,  cx+8, '|');        GUNO_;
 
    GUNO; mvaddch (r+10, cx+2, '|');        GUNO_;
    BDR;  mvaddch (r+10, cx+3, 'H');        BDR_;
    INT1; mvaddch (r+10, cx+4, '[');        INT1_;
    INT2; mvaddch (r+10, cx+5, ACS_BLOCK); INT2_;
    INT1; mvaddch (r+10, cx+6, ']');        INT1_;
    BDR;  mvaddch (r+10, cx+7, 'H');        BDR_;
    GUNO; mvaddch (r+10, cx+8, '|');        GUNO_;
 
    GUNO; mvaddch (r+11, cx+2, '|');        GUNO_;
    BDR;  mvaddch (r+11, cx+3, 'H');        BDR_;
    INT1; mvaddch (r+11, cx+4, '[');        INT1_;
    INT2; mvaddch (r+11, cx+5, ACS_BLOCK); INT2_;
    INT1; mvaddch (r+11, cx+6, ']');        INT1_;
    BDR;  mvaddch (r+11, cx+7, 'H');        BDR_;
    GUNO; mvaddch (r+11, cx+8, '|');        GUNO_;
 
    GUNO; mvaddch (r+12, cx+2, '|');        GUNO_;
    BDR;  mvaddch (r+12, cx+3, 'H');        BDR_;
    INT1; mvaddch (r+12, cx+4, '[');        INT1_;
    INT2; mvaddch (r+12, cx+5, ACS_BLOCK); INT2_;
    INT1; mvaddch (r+12, cx+6, ']');        INT1_;
    BDR;  mvaddch (r+12, cx+7, 'H');        BDR_;
    GUNO; mvaddch (r+12, cx+8, '|');        GUNO_;

    GUNO; mvprintw(r+13, cx+3,        "\\###/");           GUNO_;

    #undef BDR
    #undef BDR_
    #undef GRY
    #undef GRY_
    #undef SCP
    #undef SCP_
    #undef DOT
    #undef DOT_
    #undef HND
    #undef HND_
    #undef HNDS
    #undef HNDS_
    #undef GUNO
    #undef GUNO_
}