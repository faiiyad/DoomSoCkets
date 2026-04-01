#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "render.h"
#include "defs.h"
#include "map.h"
#include "ray.h"
#include "gun.h"
#include "entity.h"

// z_buffer — one depth value per screen column, filled by draw_walls
static double *z_buf = NULL;
static int     z_buf_cols = 0;

static void ensure_zbuf(int cols)
{
    if (cols != z_buf_cols) {
        free(z_buf);
        z_buf = malloc(sizeof(double) * cols);
        z_buf_cols = cols;
    }
}

/* ── ceiling and floor ──────────────────────────────────────────────────── */
static void draw_ceiling_floor(int rows, int cols)
{
    int half = rows / 2;
    for (int y = 0; y < rows; y++) {
        int pair = (y < half) ? CP_CEIL : CP_FLOOR;
        double t = (y < half)
            ? (double)(half - y) / half
            : (double)(y - half) / half;
        int idx = (int)(t * (N_SHADES - 1));
        char ch = SHADES[idx];
        attron(COLOR_PAIR(pair));
        mvhline(y, 0, ch, cols);
        attroff(COLOR_PAIR(pair));
    }
}

/* ── wall columns — also fills z_buf ───────────────────────────────────── */
static void draw_walls(Player *p, int rows, int cols)
{
    ensure_zbuf(cols);
    int half = rows / 2;
    for (int x = 0; x < cols; x++) {
        double ray_angle = (p->angle - FOV / 2.0)
                           + ((double)x / cols) * FOV;
        int side, wtype;
        double dist = cast_ray(p->x, p->y, ray_angle, &side, &wtype);

        dist *= cos(ray_angle - p->angle);
        if (dist < 0.001) dist = 0.001;

        z_buf[x] = dist;   // ← store for entity occlusion

        int wall_h = (int)(rows / dist);
        if (wall_h > rows) wall_h = rows;

        int top    = half - wall_h / 2;
        int bottom = half + wall_h / 2;

        double bright = 1.0 - (dist / MAX_DEPTH);
        if (bright < 0) bright = 0;
        if (side == 1) bright *= 0.6;

        int shade_idx = (int)(bright * (N_SHADES - 1));
        char wch = SHADES[shade_idx];

        int cpair;
        switch (wtype) {
            case 2:  cpair = CP_WALL2; break;
            case 3:  cpair = CP_WALL3; break;
            case 4:  cpair = CP_WALL4; break;
            default: cpair = (side == 1) ? CP_XWALL : CP_WALL1; break;
        }

        attron(COLOR_PAIR(cpair));
        for (int y = top; y < bottom; y++) {
            if (y >= 0 && y < rows)
                mvaddch(y, x, wch);
        }
        attroff(COLOR_PAIR(cpair));
    }
}

/* ── HUD ────────────────────────────────────────────────────────────────── */
static void draw_hud(Player *p, int rows)
{
    attron(COLOR_PAIR(CP_HUD) | A_BOLD);
    mvprintw(rows - 1, 0,
        " [WASD] Move  [K/L] Turn  [M] Map  [Q] Quit  |  pos(%.1f,%.1f) ang:%.0f° ",
        p->x, p->y, p->angle * 180.0 / M_PI);
    attroff(COLOR_PAIR(CP_HUD) | A_BOLD);
}

/* ── crosshair ──────────────────────────────────────────────────────────── */
static void draw_crosshair(int rows, int cols)
{
    int half = rows / 2;
    attron(A_BOLD | COLOR_PAIR(CP_MAP_P));
    
    mvaddch(half, cols / 2,     'x');
    // mvaddch(half + 1, cols / 2,     '|');
    mvaddch(half,     cols / 2 - 2, '[');
    // mvaddch(half,     cols / 2 - 1, ' ');
    // mvaddch(half,     cols / 2 + 1, ' ');
    mvaddch(half,     cols / 2 + 2, ']');
    attroff(A_BOLD | COLOR_PAIR(CP_MAP_P));
}

/* ── minimap border ─────────────────────────────────────────────────────── */
static void draw_map_border(int ox, int oy, Player *p)
{
    int border_w = MAP_W * 2 + 2;
    int border_h = MAP_H + 2;

    int top   = oy - 1;
    int bot   = oy + border_h - 2;
    int left  = ox - 1;
    int right = ox + border_w - 2;

    char coords[32];
    snprintf(coords, sizeof(coords), " x:%.1f y:%.1f ", p->x, p->y);
    int clen     = (int)strlen(coords);
    int leg_w    = clen + 2;
    if (leg_w < 14) leg_w = 14;
    int leg_left = right - leg_w;
    int leg_bot  = bot + 2;

    attron(COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    mvaddwstr(top - 1, left,  L"╔");
    mvaddwstr(top - 1, right, L"╗");
    const char *title = " RADAR ";
    int tlen = (int)strlen(title);
    int tmid = left + (border_w - tlen) / 2;
    for (int i = left + 1; i < right; i++) {
        move(top - 1, i);
        if (i >= tmid && i < tmid + tlen) {
            attroff(COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
            attron(COLOR_PAIR(CP_MAP_P) | A_BOLD);
            addch(title[i - tmid]);
            attron(COLOR_PAIR(CP_MAP_P) | A_BOLD);
            attron(COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
        }
        else                              addwstr(L"═");
    }

    mvaddwstr(top, left,  L"╠");
    mvaddwstr(top, right, L"╣");
    for (int i = left + 1; i < right; i++)
        mvaddwstr(top, i, L"═");

    attroff(COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    attron(COLOR_PAIR(CP_MAP_BDR));
    for (int i = 0; i < MAP_H; i++) {
        mvaddwstr(oy + i, left,  L"║");
        mvaddwstr(oy + i, right, L"║");
    }

    mvaddwstr(bot, left,     L"╚");
    mvaddwstr(bot, leg_left, L"╦");
    mvaddwstr(bot, right,    L"╣");
    for (int i = left     + 1; i < leg_left; i++) mvaddwstr(bot, i, L"═");
    for (int i = leg_left + 1; i < right;    i++) mvaddwstr(bot, i, L"═");

    mvaddwstr(bot + 1, leg_left, L"║");
    mvaddwstr(bot + 1, right,    L"║");

    move(bot + 1, leg_left + 1);
    for (int i = 0; i < clen; i++) addch(coords[i]);
    for (int i = leg_left + 1 + clen; i < right; i++) addch(' ');

    mvaddwstr(leg_bot, leg_left, L"╚");
    mvaddwstr(leg_bot, right,    L"╝");
    for (int i = leg_left + 1; i < right; i++)
        mvaddwstr(leg_bot, i, L"═");

    attroff(COLOR_PAIR(CP_MAP_BDR));

    int vmid = oy + MAP_H / 2;
    int hmid = left + border_w / 2;
    attron(COLOR_PAIR(CP_MAP_P) | A_BOLD);
    mvaddch(top - 1, hmid,      'N');
    mvaddch(top,     hmid,      'S');
    mvaddch(vmid,    left  + 1, 'W');
    mvaddch(vmid,    right - 1, 'E');
    attroff(COLOR_PAIR(CP_MAP_P) | A_BOLD);
}

/* ── minimap tiles ──────────────────────────────────────────────────────── */
static void draw_map_tiles(int ox, int oy, int rows, int cols)
{
    int mscale = 2;
    for (int my = 0; my < MAP_H; my++) {
        for (int mx = 0; mx < MAP_W; mx++) {
            int sy = oy + my;
            int sx = ox + mx * mscale;
            if (sy >= rows - 1 || sx + 1 >= cols) continue;
            if (map_solid(mx, my)) {
                int t = map_type(mx, my);
                int cp = (t == 2) ? CP_WALL2_M
                       : (t == 3) ? CP_WALL3_M
                       : (t == 4) ? CP_WALL4_M : CP_WALL1_M;
                attron(COLOR_PAIR(cp) | A_BOLD);
                mvaddstr(sy, sx, "  ");
                attroff(COLOR_PAIR(cp) | A_BOLD);
            } else {
                attron(COLOR_PAIR(CP_MAP_EMPTY));
                mvaddstr(sy, sx, "  ");
                attroff(COLOR_PAIR(CP_MAP_EMPTY));
            }
        }
    }
}

/* ── minimap player + entities ───────────────────────────────────────────── */
static void draw_map_player(Player *p, int ox, int oy, int rows, int cols)
{
    // draw entities on map first so player overdraw them
    for (int i = 0; i < num_entities; i++) {
        Entity *e = &entities[i];
        int ey = oy + (int)e->y;
        int ex = ox + (int)(e->x) * 2;
        if (ey > 0 && ey < rows - 1 && ex >= 0 && ex < cols) {
            attron(COLOR_PAIR(CP_ENTITY_R) | A_BOLD);
            mvaddch(ey, ex, 'X');
            attroff(COLOR_PAIR(CP_ENTITY_R) | A_BOLD);
        }
    }

    int py_m = oy + (int)p->y;
    int px_m = ox + (int)(p->x) * 2;
    if (py_m <= 0 || py_m >= rows - 1 || px_m < 0 || px_m >= cols) return;

    double a = fmod(p->angle, 2 * M_PI);
    if (a < 0) a += 2 * M_PI;

    const wchar_t *arrow;
    if      (a < 1*M_PI/8)  arrow = L"→";
    else if (a < 3*M_PI/8)  arrow = L"↘";
    else if (a < 5*M_PI/8)  arrow = L"↓";
    else if (a < 7*M_PI/8)  arrow = L"↙";
    else if (a < 9*M_PI/8)  arrow = L"←";
    else if (a < 11*M_PI/8) arrow = L"↖";
    else if (a < 13*M_PI/8) arrow = L"↑";
    else if (a < 15*M_PI/8) arrow = L"↗";
    else                    arrow = L"→";

    attron(COLOR_PAIR(CP_MAP_P) | A_BOLD);
    mvaddwstr(py_m, px_m, arrow);
    attroff(COLOR_PAIR(CP_MAP_P) | A_BOLD);
}

/* ── minimap entry ──────────────────────────────────────────────────────── */
static void draw_minimap(Player *p, int rows, int cols)
{
    int ox = cols-35, oy = 3;
    draw_map_border(ox, oy, p);
    draw_map_tiles (ox, oy, rows, cols);
    draw_map_player(p, ox, oy, rows, cols);
}

/* ── main render entry ──────────────────────────────────────────────────── */
void render(Player *p, int show_map)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (rows < 4 || cols < 8) return;

    draw_ceiling_floor(rows, cols);
    draw_walls        (p, rows, cols);
    entities_draw      (p, z_buf, rows, cols);  // after walls, before HUD/gun
    draw_hud          (p, rows);
    draw_crosshair    (rows, cols);
    if (show_map)
        draw_minimap  (p, rows, cols);
    draw_gun          (rows, cols);

    refresh();
}