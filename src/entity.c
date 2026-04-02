#include <math.h>
#include <stdlib.h>
#include <string.h>
#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include "entity.h"
#include "defs.h"
#include "map.h"

Entity entities[MAX_ENTITIES];
int   num_entities = 0;

// ── imp art 10 wide x 16 tall ─────────────────────────────────────────────
// Rows 0-3:  head (rounded silhouette)
// Row  4:    neck
// Rows 5-9:  torso / shoulders
// Rows 10-15: legs
// At SPR_SCALE=0.50, rows=40:
//   1:1 detail at dist ~1.25, solid blob fallback below dist ~13

// ── FRONT ─────────────────────────────────────────────────────────────────

static const char *sprite_front[SPR_H] = {
    " DDRRRRRRRRDD ",
    "DORWRRRRRRWROD",
    "DORWRRRRRRWROD",
    " DRRRRRRRRRRD ",
    "    DRRRRD    ",
    "  DORRRRRROD  ",
    "DORRRRRRRRRROD",
    "DODRRRRRRRRDOD",
    "DRRRRRRRRRRRRD",
    " DORRRRRRRROD ",
    "  DOR    ROD  ",
    " DORR    RROD ",
    " DORR    RROD ",
    "DORR      RROD",
    "DORR      RROD",
    "DRRR      RRRD",
};

static const char *sprite_fside[SPR_H] = {
    " DDOORRRRRRRD ",
    "DDOORRWRRRRWRD",
    "DDOORRWRRRRWRD",
    " DDOORRRRRRRD ",
    "    DDORRRD    ",
    "  DDOORRRRRD  ",
    "DDOORRRRRRRRRD",
    "DDOORRRRRRRRRD",
    "DDOORRRRRRRRRD",
    "  DDOORRRRRD  ",
    "  DOR     DRR  ",
    "  DORR    DRRR ",
    "  DORR    DRRR ",
    " DORR    DRRD",
    " DORR    DRRD",
    " DORR    DRRD",
};

static const char *sprite_side[SPR_H] = {
    " DDDDDDOORRRR ",
    "DDDDDDOOWRRRRW",
    "DDDDDDOOWRRRRW",
    " DDDDDDOORRRR ",
    "    DDDDOOR    ",
    "  DDDDDDRRRR  ",
    "DDDDDDDORRRRRR",
    "DDDDDDORRRRRRR",
    "DDDDDDDORRRRRR",
    "  DDDDDDRRRR  ",
    "  DDOR  DDOR  ",
    "  DDDOR  DDOOR",
    "  DDDOR  DDOOR",
    " DDOOR  DDOOR ",
    " DDOOR  DDOOR ",
    " DDOOR  DDOOR ",
};

static const char *sprite_bside[SPR_H] = {
    " DDDDDDDDOORR ",
    "DDDDDDDDOORRWR",
    "DDDDDDDDOORRWR",
    " DDDDDDDDOORR ",
    "    DDDDOOR   ",
    "  DDDDDDOORR  ",
    "DDDDDDDDDOORRR",
    "DDDDDDDDDOORRR",
    "DDDDDDDDDOORRR",
    "  DDDDDDOORR  ",
    "  DDO     DOR  ",
    "  DDDO    DDDO ",
    "  DDDO    DDDO ",
    " DDDO    DDDO",
    " DDDO    DDDO",
    " DDDO    DDDO",
};

static const char *sprite_back[SPR_H] = {
    " DDDDDDDDDDDD ",
    "DDDDDDDDDDDDDD",
    "ODDDDDDDDDDDDO",
    " ODDDDDDDDDDO ",
    "    DDDDDD    ",
    "  ODDDDDDDDO  ",
    "ODDDDDDDDDDDDO",
    "ODDDDDDDDDDDDO",
    "ODDDDDDDDDDDDO",
    " ODDDDDDDDDDO ",
    "  ODD    DDO  ",
    " ODDD    DDDO ",
    " ODDD    DDDO ",
    "ODDD      DDDO",
    "ODDD      DDDO",
    "ODDD      DDDO",
};

static int entity_color(Entity *e, char c)
{
    switch (c) {
        case 'R': return e->col + 1;
        case 'O': return e->col + 2;
        case 'D': return e->col + 3;
        case 'W': return CP_ENTITY_W;
        default:  return e->col + 1;
    }
}

static const char **sprite_dir[5] = {
    (const char **)sprite_front,
    (const char **)sprite_fside,
    (const char **)sprite_side,
    (const char **)sprite_bside,
    (const char **)sprite_back,
};

// ── scale constants ────────────────────────────────────────────────────────
// walls:   wall_h = rows * 0.50 / dist
// entities: spr_h  = rows * 0.50 / dist  (imp is roughly wall height)
// 1:1 detail at dist = rows * SPR_SCALE / SPR_H = 40 * 0.50 / 16 = 1.25
#define SPR_SCALE    0.50
#define CHAR_ASPECT  0.65  // terminal chars ~1.5x taller than wide

void entities_init(double spawn_x, double spawn_y)
{
    memset(entities, 0, sizeof(entities));
    entities[0].id     = 3;
    entities[0].x      = spawn_x;
    entities[0].y      = spawn_y;
    entities[0].angle  = 0.0;
    entities[0].health = 10;
    entities[0].col    = CP_ENTITY_B;
    num_entities       = 1;
}

void entities_update(Player *p, int input)
{
    (void)p;
    for (int i = 0; i < num_entities; i++) {
        Entity *e = &entities[i];

        double spd = 0.08;
        double rot = 0.52;

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

#define SOLID_FALLBACK_H 8

void entities_draw(Player *p, double *z_buf, int rows, int cols)
{
    // sort back to front
    for (int i = 0; i < num_entities; i++) {
        double dx = entities[i].x - p->x;
        double dy = entities[i].y - p->y;
        entities[i].dist = dx*dx + dy*dy;
    }
    for (int i = 1; i < num_entities; i++) {
        Entity tmp = entities[i];
        int j = i - 1;
        while (j >= 0 && entities[j].dist < tmp.dist) {
            entities[j+1] = entities[j];
            j--;
        }
        entities[j+1] = tmp;
    }

    double dir_x   =  cos(p->angle);
    double dir_y   =  sin(p->angle);
    double plane_x = -dir_y * tan(FOV / 2.0);
    double plane_y =  dir_x * tan(FOV / 2.0);

    for (int i = 0; i < num_entities; i++) {
        Entity *e = &entities[i];

        double sx  = e->x - p->x;
        double sy  = e->y - p->y;
        double inv = 1.0 / (plane_x * dir_y - dir_x * plane_y);
        double tx  = inv * ( dir_y * sx - dir_x * sy);
        double tz  = inv * (-plane_y * sx + plane_x * sy);

        if (tz <= 0.1) continue;

        int screen_x = (int)((cols / 2) * (1.0 + tx / tz));
        int entity_h = (int)(rows * SPR_SCALE / tz);
        int entity_w = (int)(entity_h * ((double)SPR_W / SPR_H) * CHAR_ASPECT);

        if (entity_h < 1) entity_h = 1;
        if (entity_w < 1) entity_w = 1;

        int draw_top  = rows / 2 - entity_h / 2;
        int draw_left = screen_x - entity_w / 2;

        if (entity_h <= SOLID_FALLBACK_H) {
            for (int row = 0; row < entity_h; row++) {
                int sy_s = draw_top + row;
                if (sy_s < 0 || sy_s >= rows - 1) continue;
                for (int col = 0; col < entity_w; col++) {
                    int sx_s = draw_left + col;
                    if (sx_s < 0 || sx_s >= cols) continue;
                    if (tz >= z_buf[sx_s]) continue;
                    attron(COLOR_PAIR(entity_color(e, ' ')));
                    mvaddch(sy_s, sx_s, ' ');
                    attroff(COLOR_PAIR(entity_color(e, ' ')));
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

        const char **art = sprite_dir[art_idx];
        const char **clr = sprite_dir[art_idx];

        for (int row = 0; row < entity_h; row++) {
            int sy_s = draw_top + row;
            if (sy_s < 0 || sy_s >= rows - 1) continue;

            int v = (int)((double)row / entity_h * SPR_H);
            if (v < 0)      v = 0;
            if (v >= SPR_H) v = SPR_H - 1;

            for (int col = 0; col < entity_w; col++) {
                int sx_s = draw_left + col;
                if (sx_s < 0 || sx_s >= cols) continue;
                if (tz >= z_buf[sx_s]) continue;

                int u = (int)(((double)col + 0.5) / entity_w * SPR_W);
                if (mirror) u = SPR_W - 1 - u;
                if (u < 0)      u = 0;
                if (u >= SPR_W) u = SPR_W - 1;

                int row_len = (int)strlen(art[v]);
                int clr_len = (int)strlen(clr[v]);
                if (u >= row_len || u >= clr_len) continue;

                char ch    = art[v][u];
                char col_c = clr[v][u];
                if (ch == ' ' || col_c == ' ') continue;

                int pair = entity_color(e, col_c);
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(sy_s, sx_s, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }
}

void entity_upsert(int id, double x, double y, double angle, int health)
{
    // update existing
    for (int i = 0; i < num_entities; i++) {
        if (entities[i].id == id) {
            entities[i].x      = x;
            entities[i].y      = y;
            entities[i].angle  = angle;
            entities[i].health = health;
            return;
        }
    }

    // not found, add new
    if (num_entities >= MAX_ENTITIES) return;
    entities[num_entities].id     = id;
    entities[num_entities].x      = x;
    entities[num_entities].y      = y;
    entities[num_entities].angle  = angle;
    entities[num_entities].health = health;
    num_entities++;
}


void entity_remove(int id)
{
    for (int i = 0; i < num_entities; i++) {
        if (entities[i].id == id) {
            entities[i] = entities[num_entities - 1];
            num_entities--;
            return;
        }
    }
}