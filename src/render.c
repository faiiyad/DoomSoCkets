#include <math.h>
#include <ncurses.h>
#include "render.h"
#include "defs.h"
#include "map.h"
#include "ray.h"
#include "gun.h"

void render(Player *p, int show_map)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (rows < 4 || cols < 8) return;

    int half = rows / 2;

    /* ── ceiling and floor ──────────────────────────────────── */
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

    /* ── wall columns ───────────────────────────────────────── */
    for (int x = 0; x < cols; x++) {
        double ray_angle = (p->angle - FOV / 2.0)
                           + ((double)x / cols) * FOV;
        int side, wtype;
        double dist = cast_ray(p->x, p->y, ray_angle, &side, &wtype);

        dist *= cos(ray_angle - p->angle);
        if (dist < 0.001) dist = 0.001;

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

    /* ── HUD bar ────────────────────────────────────────────── */
    attron(COLOR_PAIR(CP_HUD) | A_BOLD);
    mvprintw(rows - 1, 0,
        " [WASD] Move  [</>] Turn  [M] Map  [Q] Quit  |  pos(%.1f,%.1f) ang:%.0f° ",
        p->x, p->y, p->angle * 180.0 / M_PI);
    attroff(COLOR_PAIR(CP_HUD) | A_BOLD);

    /* ── crosshair ──────────────────────────────────────────── */
    attron(A_BOLD | COLOR_PAIR(CP_MAP_P));
    mvaddch(half, cols / 2, 'X');
    attroff(A_BOLD | COLOR_PAIR(CP_MAP_P));

    /* ── minimap ────────────────────────────────────────────── */
    if (show_map) {
        int mscale = 2;
        int ox = 1, oy = 1;
        for (int my = 0; my < MAP_H; my++) {
            for (int mx = 0; mx < MAP_W; mx++) {
                int sy = oy + my;
                int sx = ox + mx * mscale;
                if (sy >= rows - 1 || sx + 1 >= cols) continue;
                if (map_solid(mx, my)) {
                    int t = map_type(mx, my);
                    int cp = (t == 2) ? CP_WALL2
                           : (t == 3) ? CP_WALL3
                           : (t == 4) ? CP_WALL4 : CP_MAP_W;
                    attron(COLOR_PAIR(cp));
                    mvaddstr(sy, sx, "[]");
                    attroff(COLOR_PAIR(cp));
                } else {
                    mvaddstr(sy, sx, "  ");
                }
            }
        }

        int py_m = oy + (int)p->y;
        int px_m = ox + (int)(p->x) * mscale;
        if (py_m > 0 && py_m < rows - 1 && px_m >= 0 && px_m < cols) {
            double a = fmod(p->angle, 2 * M_PI);
            if (a < 0) a += 2 * M_PI;

            char dir;
            if      (a < M_PI/8   || a > 15*M_PI/8) dir = '>';
            else if (a < 3*M_PI/8)                   dir = 'v';
            else if (a < 5*M_PI/8)                   dir = 'v';
            else if (a < 7*M_PI/8)                   dir = 'v';
            else if (a < 9*M_PI/8)                   dir = '<';
            else if (a < 11*M_PI/8)                  dir = '^';
            else if (a < 13*M_PI/8)                  dir = '^';
            else                                      dir = '^';

            attron(COLOR_PAIR(CP_MAP_P) | A_BOLD);
            mvaddch(py_m, px_m, dir);
            attroff(COLOR_PAIR(CP_MAP_P) | A_BOLD);
        }
    }

    /* ── weapon ─────────────────────────────────────────────── */
    draw_gun(rows, cols);

    refresh();
}