#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <math.h>
#include <stdlib.h>
#include "render.h"
#include "defs.h"
#include "map.h"
#include "ray.h"
#include "gun.h"
#include "entity.h"
#include "ui.h"

// z_buffer, stores distance for each column for entities to check against when drawing
static double *z_buf = NULL;
static int z_buf_cols = 0;

static void ensure_zbuf(int cols)
{
    if (cols != z_buf_cols) {
        free(z_buf);
        z_buf = malloc(sizeof(double) * cols);
        z_buf_cols = cols;
    }
}

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

        z_buf[x] = dist;

        int wall_h = (int)(rows / dist);
        if (wall_h > rows) wall_h = rows;

        int top    = half - wall_h / 2;
        int bottom = half + wall_h / 2;

        double bright = 1.0 - (dist * 4 / MAX_DEPTH);
        if (bright < 0.0) bright = 0.0;
        if (bright > 1.0) bright = 1.0;

        int shade_idx = (int)(bright * (N_SHADES - 1));
        char wch = SHADES[shade_idx];

        double t = 1.0 - bright;
        t = t * t * (3.0 - 2.0 * t);
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;

        int col_idx = (int)(t * (N_WALL_SHADES - 1));
        if (col_idx < 0) col_idx = 0;
        if (col_idx >= N_WALL_SHADES) col_idx = N_WALL_SHADES - 1;

        int cpair = (side == 1) ? CP_WALL_S1 + col_idx : CP_WALL_F1 + col_idx;

        // No colour shading on walls below
        // cpair = (side == 1) ? CP_XWALL : CP_WALL1;

        attron(COLOR_PAIR(cpair));
        for (int y = top; y < bottom; y++) {
            if (y >= 0 && y < rows)
                mvaddch(y, x, wch);
        }
        attroff(COLOR_PAIR(cpair));
    }
}

static void draw_crosshair(int rows, int cols)
{
    int half = rows / 2;
    attron(A_BOLD | COLOR_PAIR(CP_MAP_P));
    
    mvaddch(half, cols / 2,     'x');
    mvaddch(half,     cols / 2 - 2, '[');
    mvaddch(half,     cols / 2 + 2, ']');
    attroff(A_BOLD | COLOR_PAIR(CP_MAP_P));
}

void render(Player *p, int show_map)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (rows < 4 || cols < 8) return;

    draw_ceiling_floor(rows, cols);
    draw_walls        (p, rows, cols);
    entities_draw      (p, z_buf, rows, cols);  // after walls, before HUD/gun
    
    ui_draw_hud       (p);
    ui_draw_controls  (rows);
    ui_draw_server   (rows);
    ui_draw_eventlog (rows, cols);
    
    draw_crosshair    (rows, cols);
    if (show_map){
        ui_draw_minimap(p, rows, cols);
    }
    draw_gun          (p->cur_gun, rows, cols);

    refresh();
}