#include <math.h>
#include <stdlib.h>
#include <string.h>
#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include "sprite.h"
#include "defs.h"
#include "map.h"

Enemy enemies[MAX_SPRITES];
int   num_enemies = 0;

// ── imp art 10 wide x 16 tall ─────────────────────────────────────────────
// Rows 0-3:  head (rounded silhouette)
// Row  4:    neck
// Rows 5-9:  torso / shoulders
// Rows 10-15: legs
// At SPR_SCALE=0.50, rows=40:
//   1:1 detail at dist ~1.25, solid blob fallback below dist ~13

// ── FRONT ─────────────────────────────────────────────────────────────────
static const char *imp_front[SPR_H] = {
    " /HHHH\\ ",   // 0  head top
    "/HOOOOH\\",   // 1  eyes
    "|HHHHHH|",   // 2  face
    " \\HHHH/ ",   // 3  chin
    "  /HH\\  ",   // 4  neck
    " /HHHH\\ ",   // 5  shoulders
    "/HHHHHH\\",   // 6  chest top
    "|H/HH\\H|",   // 7  chest detail
    "|HHHHHH|",   // 8  chest bot
    " \\HHHH/ ",   // 9  waist
    " |H  H| ",   // 10 hips
    " |H  H| ",   // 11 thigh
    " /H  H\\ ",   // 12 knee
    "/H    H\\",   // 13 shin
    "|H    H|",   // 14 ankle
    "\\H    H/",   // 15 feet
};
static const char *imp_front_clr[SPR_H] = {
    " DRRRRD ",
    "DROOOODR",
    "DRRRRRRD",
    " DRRRRD ",
    "  DRRD  ",
    " DRRRRD ",
    "DRRRRRRD",
    "DRDRRDRR",
    "DRRRRRRD",
    " DRRRRD ",
    " DR  RD ",
    " DR  RD ",
    " DR  RD ",
    "DR    RD",
    "DR    RD",
    "DR    RD",
};

// ── FRONT-SIDE ────────────────────────────────────────────────────────────
static const char *imp_fside[SPR_H] = {
    " /HHHH\\ ",
    "/HOOHHH\\",   // one eye visible
    "|HHHHHH|",
    " \\HHHH/ ",
    "  /HH\\  ",
    " /HHHH\\ ",
    "/HHHHHH\\",
    "|H/HH\\H|",
    "|HHHHHH|",
    " \\HHHH/ ",
    " |H  H| ",
    " |H  H| ",
    " /H  H\\ ",
    "/H    H\\",
    "|H    H|",
    "\\H    H/",
};
static const char *imp_fside_clr[SPR_H] = {
    " DDRRRD ",
    "DDOODDRR",
    "DDRRRRRD",
    " DDRRRD ",
    "  DDRD  ",
    " DDRRRD ",
    "DDRRRRRD",
    "DDRDDDRR",
    "DDRRRRRD",
    " DDRRRD ",
    " DD  RD ",
    " DD  RD ",
    " DD  RD ",
    "DD    RD",
    "DD    RD",
    "DD    RD",
};

// ── SIDE ──────────────────────────────────────────────────────────────────
static const char *imp_side[SPR_H] = {
    "  /HHH\\ ",
    " /HOHNH\\",   // N = nose profile
    " |HHHHH|",
    "  \\HHH/ ",
    "   /HH\\ ",
    "  /HHH\\ ",
    " /HHHH\\ ",
    " |H/HH| ",
    " |HHHH| ",
    "  \\HHH/ ",
    "  |H H| ",
    "  |H H| ",
    "  /H H\\ ",
    " /H   H\\",
    " |H   H|",
    " \\H   H/",
};
static const char *imp_side_clr[SPR_H] = {
    "  DDDRD ",
    " DDODDD ",
    " DDDDDD ",
    "  DDDRD ",
    "   DDDD ",
    "  DDDRD ",
    " DDDRD  ",
    " DDDDD  ",
    " DDDDD  ",
    "  DDDRD ",
    "  DD DD ",
    "  DD DD ",
    "  DD DD ",
    " DD   DD",
    " DD   DD",
    " DD   DD",
};

// ── BACK-SIDE ─────────────────────────────────────────────────────────────
static const char *imp_bside[SPR_H] = {
    " /HHHH\\ ",
    "/HHHOHH\\",
    "|HHHHHH|",
    " \\HHHH/ ",
    "  /HH\\  ",
    " /HHHH\\ ",
    "/HHHHHH\\",
    "|HHHHHH|",
    "|HHHHHH|",
    " \\HHHH/ ",
    " |H  H| ",
    " |H  H| ",
    " /H  H\\ ",
    "/H    H\\",
    "|H    H|",
    "\\H    H/",
};
static const char *imp_bside_clr[SPR_H] = {
    " DDDDRD ",
    "DDDDDODD",
    "DDDDDDDD",
    " DDDDRD ",
    "  DDDD  ",
    " DDDDRD ",
    "DDDDDDDD",
    "DDDDDDDD",
    "DDDDDDDD",
    " DDDDDD ",
    " DD  DD ",
    " DD  DD ",
    " DD  DD ",
    "DD    DD",
    "DD    DD",
    "DD    DD",
};

// ── BACK ──────────────────────────────────────────────────────────────────
static const char *imp_back[SPR_H] = {
    " /HHHH\\ ",
    "/HHHHHH\\",
    "|HHHHHH|",
    " \\HHHH/ ",
    "  /HH\\  ",
    " /HHHH\\ ",
    "/HHHHHH\\",
    "|HHHHHH|",
    "|HHHHHH|",
    " \\HHHH/ ",
    " |H  H| ",
    " |H  H| ",
    " /H  H\\ ",
    "/H    H\\",
    "|H    H|",
    "\\H    H/",
};
static const char *imp_back_clr[SPR_H] = {
    " DDDDDD ",
    "DDDDDDDD",
    "DDDDDDDD",
    " DDDDDD ",
    "  DDDD  ",
    " DDDDDD ",
    "DDDDDDDD",
    "DDDDDDDD",
    "DDDDDDDD",
    " DDDDDD ",
    " DD  DD ",
    " DD  DD ",
    " DD  DD ",
    "DD    DD",
    "DD    DD",
    "DD    DD",
};

static int sprite_color(char c)
{
    switch (c) {
        case 'R': return CP_SPRITE_R;
        case 'D': return CP_SPRITE_D;
        case 'O': return CP_SPRITE_O;
        case 'W': return CP_SPRITE_W;
        default:  return CP_SPRITE_R;
    }
}

static const char **dir_art[5] = {
    (const char **)imp_front,
    (const char **)imp_fside,
    (const char **)imp_side,
    (const char **)imp_bside,
    (const char **)imp_back,
};
static const char **dir_clr[5] = {
    (const char **)imp_front_clr,
    (const char **)imp_fside_clr,
    (const char **)imp_side_clr,
    (const char **)imp_bside_clr,
    (const char **)imp_back_clr,
};

// ── scale constants ────────────────────────────────────────────────────────
// walls:   wall_h = rows * 0.50 / dist
// sprites: spr_h  = rows * 0.50 / dist  (imp is roughly wall height)
// 1:1 detail at dist = rows * SPR_SCALE / SPR_H = 40 * 0.50 / 16 = 1.25
#define SPR_SCALE    0.50
#define CHAR_ASPECT  0.65  // terminal chars ~1.5x taller than wide

void sprites_init(double spawn_x, double spawn_y)
{
    memset(enemies, 0, sizeof(enemies));
    enemies[0].x      = spawn_x;
    enemies[0].y      = spawn_y;
    enemies[0].angle  = 0.0;
    enemies[0].health = 100;
    enemies[0].state  = STATE_IDLE;
    enemies[0].active = 1;
    num_enemies       = 1;
}

void sprites_update(Player *p, int input)
{
    (void)p;
    for (int i = 0; i < num_enemies; i++) {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        double spd = 0.08;
        double rot = 0.12;

        if (input == 'f' || input == 'F') {
            e->angle -= rot;
            if (e->angle < 0) e->angle += 2*M_PI;
        }
        if (input == 'h' || input == 'H') {
            e->angle += rot;
            if (e->angle >= 2*M_PI) e->angle -= 2*M_PI;
        }
        if (input == 'g' || input == 'G') {
            double nx = e->x + cos(e->angle) * spd;
            double ny = e->y + sin(e->angle) * spd;
            if (!map_solid((int)nx,   (int)e->y)) e->x = nx;
            if (!map_solid((int)e->x, (int)ny))   e->y = ny;
        }
        if (input == 'b' || input == 'B') {
            double nx = e->x - cos(e->angle) * spd;
            double ny = e->y - sin(e->angle) * spd;
            if (!map_solid((int)nx,   (int)e->y)) e->x = nx;
            if (!map_solid((int)e->x, (int)ny))   e->y = ny;
        }
        if (input == 'v' || input == 'V') {
            double nx = e->x + cos(e->angle - M_PI/2) * spd;
            double ny = e->y + sin(e->angle - M_PI/2) * spd;
            if (!map_solid((int)nx,   (int)e->y)) e->x = nx;
            if (!map_solid((int)e->x, (int)ny))   e->y = ny;
        }
        if (input == 'n' || input == 'N') {
            double nx = e->x + cos(e->angle + M_PI/2) * spd;
            double ny = e->y + sin(e->angle + M_PI/2) * spd;
            if (!map_solid((int)nx,   (int)e->y)) e->x = nx;
            if (!map_solid((int)e->x, (int)ny))   e->y = ny;
        }
    }
}

#define SOLID_FALLBACK_H 3

void sprites_draw(Player *p, double *z_buf, int rows, int cols)
{
    // sort back to front
    for (int i = 0; i < num_enemies; i++) {
        double dx = enemies[i].x - p->x;
        double dy = enemies[i].y - p->y;
        enemies[i].dist = dx*dx + dy*dy;
    }
    for (int i = 1; i < num_enemies; i++) {
        Enemy tmp = enemies[i];
        int j = i - 1;
        while (j >= 0 && enemies[j].dist < tmp.dist) {
            enemies[j+1] = enemies[j];
            j--;
        }
        enemies[j+1] = tmp;
    }

    double dir_x   =  cos(p->angle);
    double dir_y   =  sin(p->angle);
    double plane_x = -dir_y * tan(FOV / 2.0);
    double plane_y =  dir_x * tan(FOV / 2.0);

    for (int i = 0; i < num_enemies; i++) {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        double sx  = e->x - p->x;
        double sy  = e->y - p->y;
        double inv = 1.0 / (plane_x * dir_y - dir_x * plane_y);
        double tx  = inv * ( dir_y * sx - dir_x * sy);
        double tz  = inv * (-plane_y * sx + plane_x * sy);

        if (tz <= 0.1) continue;

        int screen_x = (int)((cols / 2) * (1.0 + tx / tz));
        int sprite_h = (int)(rows * SPR_SCALE / tz);
        int sprite_w = (int)(sprite_h * ((double)SPR_W / SPR_H) * CHAR_ASPECT);

        if (sprite_h < 1) sprite_h = 1;
        if (sprite_w < 1) sprite_w = 1;

        int draw_top  = rows / 2 - sprite_h / 2;
        int draw_left = screen_x - sprite_w / 2;

        if (sprite_h < SOLID_FALLBACK_H) {
            for (int row = 0; row < sprite_h; row++) {
                int sy_s = draw_top + row;
                if (sy_s < 0 || sy_s >= rows - 1) continue;
                for (int col = 0; col < sprite_w; col++) {
                    int sx_s = draw_left + col;
                    if (sx_s < 0 || sx_s >= cols) continue;
                    if (tz >= z_buf[sx_s]) continue;
                    attron(COLOR_PAIR(CP_SPRITE_R));
                    mvaddch(sy_s, sx_s, ' ');
                    attroff(COLOR_PAIR(CP_SPRITE_R));
                }
            }
            continue;
        }

        double angle_to_player = atan2(p->y - e->y, p->x - e->x);
        double diff = fmod((angle_to_player - e->angle) + 2*M_PI, 2*M_PI);
        int dir8    = (int)((diff + M_PI/8.0) / (M_PI/4.0)) % 8;

        int art_idx = 0, mirror = 0;
        switch (dir8) {
            case 0: art_idx=0; mirror=0; break;
            case 1: art_idx=1; mirror=0; break;
            case 2: art_idx=2; mirror=0; break;
            case 3: art_idx=3; mirror=0; break;
            case 4: art_idx=4; mirror=0; break;
            case 5: art_idx=3; mirror=1; break;
            case 6: art_idx=2; mirror=1; break;
            case 7: art_idx=1; mirror=1; break;
        }

        const char **art = dir_art[art_idx];
        const char **clr = dir_clr[art_idx];

        for (int row = 0; row < sprite_h; row++) {
            int sy_s = draw_top + row;
            if (sy_s < 0 || sy_s >= rows - 1) continue;

            int v = (int)((double)row / sprite_h * SPR_H);
            if (v < 0)      v = 0;
            if (v >= SPR_H) v = SPR_H - 1;

            for (int col = 0; col < sprite_w; col++) {
                int sx_s = draw_left + col;
                if (sx_s < 0 || sx_s >= cols) continue;
                if (tz >= z_buf[sx_s]) continue;

                int u = (int)(((double)col + 0.5) / sprite_w * SPR_W);
                if (mirror) u = SPR_W - 1 - u;
                if (u < 0)      u = 0;
                if (u >= SPR_W) u = SPR_W - 1;

                int row_len = (int)strlen(art[v]);
                int clr_len = (int)strlen(clr[v]);
                if (u >= row_len || u >= clr_len) continue;

                char ch    = art[v][u];
                char col_c = clr[v][u];
                if (ch == ' ' || col_c == ' ') continue;

                int pair = sprite_color(col_c);
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(sy_s, sx_s, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}