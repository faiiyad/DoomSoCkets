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

// ── sprite art — 5 directions, mirrored for the other 3 ──────────────────
// ' ' = transparent cell, any other char gets drawn
// Each row is exactly SPR_W=7 chars wide

static const char *imp_front[SPR_H] = {
    " HHHH",
    " HHHH",
    " HHHH",
    "  HH",
    "HHHHHH",
    "H HH H",
    " HHHH",
    " H  H",
    " H  H",
};
static const char *imp_fside[SPR_H] = {  // front-quarter side
    " HHHH",
    " HHHH",
    " HHHH",
    "  HH",
    "HHHHHH",
    "H HH H",
    " HHHH",
    " H  H",
    " H  H",
};
static const char *imp_side[SPR_H] = {
    " HHHH",
    " HHHH",
    " HHHH",
    "  HH",
    "HHHHHH",
    "H HH H",
    " HHHH",
    " H  H",
    " H  H",
};
static const char *imp_bside[SPR_H] = {  // back-quarter side
    " HHHH",
    " HHHH",
    " HHHH",
    "  HH",
    "HHHHHH",
    "H HH H",
    " HHHH",
    " H  H",
    " H  H",
};
static const char *imp_back[SPR_H] = {
    " HHHH",
    " HHHH",
    " HHHH",
    "  HH",
    "HHHHHH",
    "H HH H",
    " HHHH",
    " H  H",
    " H  H",
};

// color maps parallel to art — same rules: space=transparent
// R=red body, D=dark/shadow, O=orange highlight, W=white eye
static const char *imp_front_clr[SPR_H] = {
    " RRRR",
    " WRRW",
    " RRRR",
    "  RR",
    "RRRRRR",
    "R RR R",
    " RRRR",
    " R  R",
    " R  R",
};
static const char *imp_fside_clr[SPR_H] = {
    " DDRR",
    " DOWR",
    " DORR",
    "  OR",
    "DDORRR",
    "D OR R",
    " DORR",
    " D  R",
    " D  R",
};
static const char *imp_side_clr[SPR_H] = {
    " DDDR",
    " DDOW",
    " DDOR",
    "  DO",
    "DDDORR",
    "D DO R",
    " DDOR",
    " D  O",
    " D  O",
};
static const char *imp_bside_clr[SPR_H] = {
    " DDDD",
    " DDDO",
    " DDDO",
    "  DD",
    "DDDDOR",
    "D DD R",
    " DDDO",
    " D  O",
    " D  O",
};
static const char *imp_back_clr[SPR_H] = {
    " DDDD",
    " DDDD",
    " DDDD",
    "  DD",
    "DDDDDD",
    "D DD D",
    " DDDD",
    " D  D",
    " D  D",
};

// ── color resolver ────────────────────────────────────────────────────────
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

// ── direction table — 5 unique arts, indexed 0..4 ─────────────────────────
// dir 0=front 1=fside 2=side 3=bside 4=back 5=bside(m) 6=side(m) 7=fside(m)
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

// ── init ──────────────────────────────────────────────────────────────────
void sprites_init(double spawn_x, double spawn_y)
{
    memset(enemies, 0, sizeof(enemies));
    enemies[0].x       = spawn_x;
    enemies[0].y       = spawn_y;
    enemies[0].angle   = 0.0;
    enemies[0].health  = 100;
    enemies[0].state   = STATE_IDLE;
    enemies[0].active  = 1;
    num_enemies        = 1;
}

// ── update — manual control via HBNM ─────────────────────────────────────
void sprites_update(Player *p, int input)
{
    (void)p;
    for (int i = 0; i < num_enemies; i++) {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        double spd = 0.08;
        double rot = 0.12;
        
        if (input == 'v' || input == 'V') {
            e->angle -= rot;
            if (e->angle < 0)        e->angle += 2*M_PI;
        }
        if (input == 'g' || input == 'G') {   // n=forward m=back — avoids map toggle
            double nx = e->x + cos(e->angle) * spd;
            double ny = e->y + sin(e->angle) * spd;
            if (!map_solid((int)nx, (int)e->y)) e->x = nx;
            if (!map_solid((int)e->x, (int)ny)) e->y = ny;
        }
        if (input == 'b' || input == 'B') {
            double nx = e->x - cos(e->angle) * spd;
            double ny = e->y - sin(e->angle) * spd;
            if (!map_solid((int)nx, (int)e->y)) e->x = nx;
            if (!map_solid((int)e->x, (int)ny)) e->y = ny;
        }
        if (input == 'n' || input == 'N') {
            e->angle += rot;
            if (e->angle >= 2*M_PI)  e->angle -= 2*M_PI;
        }

    }
}

// ── draw ──────────────────────────────────────────────────────────────────
void sprites_draw(Player *p, double *z_buf, int rows, int cols)
{
    // ── sort by distance farthest first ───────────────────────────────────
    for (int i = 0; i < num_enemies; i++) {
        if (!enemies[i].active) continue;
        double dx = enemies[i].x - p->x;
        double dy = enemies[i].y - p->y;
        enemies[i].dist = dx*dx + dy*dy;
    }
    // simple insertion sorts, so it draws farther enemies first
    for (int i = 1; i < num_enemies; i++) {
        Enemy tmp = enemies[i];
        int j = i - 1;
        while (j >= 0 && enemies[j].dist < tmp.dist) {
            enemies[j+1] = enemies[j];
            j--;for MA
        }
        enemies[j+1] = tmp;
    }

    double dir_x = cos(p->angle);
    double dir_y = sin(p->angle);
    // camera plane — perpendicular, length tied to FOV
    double plane_x = -dir_y * tan(FOV / 2.0);
    double plane_y =  dir_x * tan(FOV / 2.0);
    // [plane|dir] is a matrix which defines a camera space

    for (int i = 0; i < num_enemies; i++) {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        // sx and sy is difference between enemy and player in the world space
        double sx = e->x - p->x;
        double sy = e->y - p->y;

        // inv = 1/detA where A is the camera space matrix
        double inv = 1.0 / (plane_x * dir_y - dir_x * plane_y);
        // convert sx and sy into the camera space
        // tx is horizontal difference from the player perspective
        // tz is depth from player persepctive
        double tx  = inv * ( dir_y * sx - dir_x * sy);
        double tz  = inv * (-plane_y * sx + plane_x * sy);
        if (tz <= 0.1) continue; // behind camera

        // screen_x is which column the sprite is centered, using slope of tx/tz
        int screen_x  = (int)((cols / 2) * (1.0 + tx / tz));
        int sprite_h = abs((int)(rows / tz));
        int sprite_w = (sprite_h + 1) / 2;  // chars are ~2:1 so halve width
        if (sprite_w < 1) sprite_w = 1;
        if (sprite_h < 1) sprite_h = 1;

        //top left coordinate of where to draw sprite
        int draw_top  = rows / 2 - sprite_h / 2;
        int draw_left = screen_x - sprite_w / 2;

        // ── pick direction (0-7) then map to art index + mirror flag ──────
        double angle_to_player = atan2(p->y - e->y, p->x - e->x);
        double diff = fmod((angle_to_player - e->angle) + 2*M_PI, 2*M_PI);
        int dir8    = (int)((diff + M_PI/8.0) / (M_PI/4.0)) % 8;

        // map dir8 → art index (0-4) + mirror
        // 0=front 1=fside 2=side 3=bside 4=back 5=bside(m) 6=side(m) 7=fside(m)
        int art_idx = 0;
        int mirror  = 0;
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

        // ── draw column by column ─────────────────────────────────────────
        for (int row = 0; row < sprite_h; row++) {
            int sy_screen = draw_top + row;
            if (sy_screen < 0 || sy_screen >= rows - 1) continue;

            int v = (int)((double)row / sprite_h * SPR_H);
            if (v < 0) v = 0;
            if (v >= SPR_H) v = SPR_H - 1;

            for (int col = 0; col < sprite_w; col++) {
                int sx_screen = draw_left + col;
                if (sx_screen < 0 || sx_screen >= cols) continue;
                if (tz >= z_buf[sx_screen]) continue;

                int u = (int)(((double)col + 0.5) / sprite_w * SPR_W);
                if (mirror) u = SPR_W - 1 - u;
                if (u < 0) u = 0;
                if (u >= SPR_W) u = SPR_W - 1;

                // bounds check the row length before indexing
                int row_len = (int)strlen(art[v]);
                int clr_len = (int)strlen(clr[v]);
                if (u >= row_len) continue;   // out of bounds = transparent
                if (u >= clr_len) continue;

                char ch    = art[v][u];
                char col_c = clr[v][u];
                if (ch == ' ' || col_c == ' ') continue;  // transparent

                int pair = sprite_color(col_c);
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(sy_screen, sx_screen, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}