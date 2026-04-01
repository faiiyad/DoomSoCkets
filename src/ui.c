#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "ui.h"
#include "defs.h"
#include "map.h"
#include "entity.h"

/* ══════════════════════════════════════════════════════════════════════════
   MINIMAP
   ══════════════════════════════════════════════════════════════════════════ */

static void draw_map_border(int ox, int oy, Player *p)
{
    int border_w = MAP_W * 2 + 2;
    int border_h = MAP_H + 2;

    int top      = oy - 1;
    int bot      = oy + border_h - 2;
    int left     = ox - 1;
    int right    = ox + border_w - 2;

    char coords[32];
    snprintf(coords, sizeof(coords), " x:%.1f y:%.1f ", p->x, p->y);
    int clen     = (int)strlen(coords);
    int leg_w    = clen + 2;
    if (leg_w < 14) leg_w = 14;
    int leg_left = right - leg_w;
    int leg_bot  = bot + 2;

    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    mvaddwstr(top - 1, left,  L"╔");
    mvaddwstr(top - 1, right, L"╗");
    const char *title = " RADAR ";
    int tlen = (int)strlen(title);
    int tmid = left + (border_w - tlen) / 2;
    for (int i = left + 1; i < right; i++) {
        move(top - 1, i);
        if (i >= tmid && i < tmid + tlen) {
            wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
            wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
            addch(title[i - tmid]);
            wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
            wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
        } else {
            addwstr(L"═");
        }
    }

    mvaddwstr(top, left,  L"╠");
    mvaddwstr(top, right, L"╣");
    for (int i = left + 1; i < right; i++)
        mvaddwstr(top, i, L"═");

    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR));
    for (int i = 0; i < MAP_H; i++) {
        mvaddwstr(oy + i, left,  L"║");
        mvaddwstr(oy + i, right, L"║");
    }

    mvaddwstr(bot, left,     L"╚");
    mvaddwstr(bot, leg_left, L"╦");
    mvaddwstr(bot, right,    L"╣");
    for (int i = left + 1;     i < leg_left; i++) mvaddwstr(bot, i, L"═");
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

    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR));

    int vmid = oy + MAP_H / 2;
    int hmid = left + border_w / 2;
    wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
    mvaddch(top - 1, hmid,      'N');
    mvaddch(top,     hmid,      'S');
    mvaddch(vmid,    left  + 1, 'W');
    mvaddch(vmid,    right - 1, 'E');
    wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
}

static void draw_map_tiles(int ox, int oy, int rows, int cols)
{
    int mscale = 2;
    for (int my = 0; my < MAP_H; my++) {
        for (int mx = 0; mx < MAP_W; mx++) {
            int sy = oy + my;
            int sx = ox + mx * mscale;
            if (sy >= rows - 1 || sx + 1 >= cols) continue;
            if (map_solid(mx, my)) {
                int t  = map_type(mx, my);
                int cp = (t == 2) ? CP_WALL2_M
                       : (t == 3) ? CP_WALL3_M
                       : (t == 4) ? CP_WALL4_M : CP_WALL1_M;
                wattron(stdscr, COLOR_PAIR(cp) | A_BOLD);
                mvwaddstr(stdscr, sy, sx, "  ");
                wattroff(stdscr, COLOR_PAIR(cp) | A_BOLD);
            } else {
                wattron(stdscr, COLOR_PAIR(CP_MAP_EMPTY));
                mvwaddstr(stdscr, sy, sx, "  ");
                wattroff(stdscr, COLOR_PAIR(CP_MAP_EMPTY));
            }
        }
    }
}

static void draw_map_player(Player *p, int ox, int oy, int rows, int cols)
{
    for (int i = 0; i < num_entities; i++) {
        Entity *e = &entities[i];
        int ey = oy + (int)e->y;
        int ex = ox + (int)(e->x) * 2;
        if (ey > 0 && ey < rows - 1 && ex >= 0 && ex < cols) {
            wattron(stdscr, COLOR_PAIR(CP_ENTITY_R) | A_BOLD);
            mvwaddch(stdscr, ey, ex, 'X');
            wattroff(stdscr, COLOR_PAIR(CP_ENTITY_R) | A_BOLD);
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

    wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
    mvaddwstr(py_m, px_m, arrow);
    wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
}

void ui_draw_minimap(Player *p, int rows, int cols)
{
    int ox = cols - (MAP_W * 2 + 2) - 2;
    if (ox < 0) ox = 0;
    int oy = 3;
    draw_map_border(ox, oy, p);
    draw_map_tiles (ox, oy, rows, cols);
    draw_map_player(p, ox, oy, rows, cols);
}

/* ══════════════════════════════════════════════════════════════════════════
   HUD — horizontal bar, top of screen
   ══════════════════════════════════════════════════════════════════════════
   Row 0:  ╔════╦══ HP ═══════════╦══ GUN ═══════╦══ KILLS ══╗
   Row 1:  ║ P1 ║ [████████░░] 75 ║ M16   12dmg  ║     0     ║
   Row 2:  ╚════╩════════════════╩═════════════╩═══════════╝
   ══════════════════════════════════════════════════════════════════════════ */

// column layout — all measured from col 0
#define P1_INNER    4    // width of P1 cell content
#define HP_INNER    16   // width of HP cell content
#define GUN_INNER   14   // width of GUN cell content
#define KIL_INNER   9    // width of KILLS cell content
#define BAR_W       10   // filled chars in health bar

// derived positions
#define HUD_L_SPACE  2   // 2 column gap from left edge
#define HUD_T_SPACE   1   // 1 row gap from top edge

#define P1_L    HUD_L_SPACE
#define P1_R    (P1_L  + P1_INNER  + 1)
#define HP_L    P1_R
#define HP_R    (HP_L  + HP_INNER  + 1)
#define GUN_L   HP_R
#define GUN_R   (GUN_L + GUN_INNER + 1)
#define KIL_L   GUN_R
#define KIL_R   (KIL_L + KIL_INNER + 1)

// draw a ═ run from col a+1 to col b-1
static void hud_fill_hline(int row, int a, int b)
{
    for (int i = a + 1; i < b; i++)
        mvaddwstr(row, i, L"═");
}

// write a section title centred between col a and col b on row, with ═ padding
static void hud_title(int row, int a, int b, const char *title)
{
    int inner = b - a - 1;
    int tlen  = (int)strlen(title);
    int pad   = (inner - tlen) / 2;
    int start = a + 1 + pad;

    for (int i = a + 1; i < b; i++) {
        move(row, i);
        int ti = i - start;
        if (ti >= 0 && ti < tlen) {
            wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
            wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
            addch(title[ti]);
            wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
            wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
        } else {
            addwstr(L"═");
        }
    }
}

void ui_draw_hud(Player *p)
{
    // ── placeholder values — wire to player fields when ready ─────────────
    int         player_num = 1;
    int         hp         = p->health;
    int         max_hp     = 100;
    const char *gun_name   = "M16";
    int         gun_dmg    = 12;
    int         kills      = 0;   // replace with p->kills

    if (hp < 0)      hp = 0;
    if (hp > max_hp) hp = max_hp;

    // ── row 0: top border ─────────────────────────────────────────────────
    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    // corners and joints
    mvaddwstr(HUD_T_SPACE + 0, P1_L,  L"╔");
    mvaddwstr(HUD_T_SPACE + 0, P1_R,  L"╦");
    mvaddwstr(HUD_T_SPACE + 0, HP_R,  L"╦");
    mvaddwstr(HUD_T_SPACE + 0, GUN_R, L"╦");
    mvaddwstr(HUD_T_SPACE + 0, KIL_R, L"╗");

    // P1 cell — plain ═ fill, no title
    hud_fill_hline(HUD_T_SPACE + 0, P1_L, P1_R);

    // titled cells
    hud_title(HUD_T_SPACE + 0, HP_L,  HP_R,  " HP ");
    hud_title(HUD_T_SPACE + 0, GUN_L, GUN_R, " GUN ");
    hud_title(HUD_T_SPACE + 0, KIL_L, KIL_R, " KILLS ");

    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    // ── row 1: content ────────────────────────────────────────────────────
    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
    mvaddwstr(HUD_T_SPACE + 1, P1_L,  L"║");
    mvaddwstr(HUD_T_SPACE + 1, P1_R,  L"║");
    mvaddwstr(HUD_T_SPACE + 1, HP_R,  L"║");
    mvaddwstr(HUD_T_SPACE + 1, GUN_R, L"║");
    mvaddwstr(HUD_T_SPACE + 1, KIL_R, L"║");
    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    // P1 label — centered in cell, accent color
    {
        char label[8];
        snprintf(label, sizeof(label), "P%d", player_num);
        int llen  = (int)strlen(label);
        int lcol  = P1_L + 1 + (P1_INNER - llen) / 2;
        for (int i = P1_L + 1; i < P1_R; i++) mvaddch(HUD_T_SPACE + 1, i, ' ');
        wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, lcol, "%s", label);
        wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
    }

    // HP bar
    {
        int filled   = (BAR_W * hp) / max_hp;
        int bar_pair = (hp > max_hp * 2/3) ? CP_WALL3
                     : (hp > max_hp * 1/3) ? CP_WALL4
                     : CP_MAP_P;
        int col = HP_L + 1;

        for (int i = HP_L + 1; i < HP_R; i++) mvaddch(HUD_T_SPACE + 1, i, ' ');

        mvaddch(HUD_T_SPACE + 1, col++, '[');

        wattron(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
        for (int i = 0; i < filled; i++)     { mvaddwstr(HUD_T_SPACE + 1, col++, L"█"); }
        wattroff(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);

        wattron(stdscr, COLOR_PAIR(CP_MAP_BDR));
        for (int i = filled; i < BAR_W; i++) { mvaddwstr(HUD_T_SPACE + 1, col++, L"░"); }
        wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR));

        mvaddch(HUD_T_SPACE + 1, col++, ']');

        wattron(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, col, " %d", hp);
        wattroff(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
    }

    // GUN cell
    {
        for (int i = GUN_L + 1; i < GUN_R; i++) mvaddch(HUD_T_SPACE + 1, i, ' ');
        char gunbuf[GUN_INNER + 1];
        snprintf(gunbuf, sizeof(gunbuf), " %-6s %3ddmg", gun_name, gun_dmg);
        wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, GUN_L + 1, "%-*.*s", GUN_INNER, GUN_INNER, gunbuf);
        wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
    }

    // KILLS cell — centered number
    {
        for (int i = KIL_L + 1; i < KIL_R; i++) mvaddch(HUD_T_SPACE + 1, i, ' ');
        char kbuf[8];
        snprintf(kbuf, sizeof(kbuf), "%d", kills);
        int klen = (int)strlen(kbuf);
        int kcol = KIL_L + 1 + (KIL_INNER - klen) / 2;
        wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, kcol, "%s", kbuf);
        wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
    }

    // ── row 2: bottom border ──────────────────────────────────────────────
    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
    mvaddwstr(HUD_T_SPACE + 2, P1_L,  L"╚");
    mvaddwstr(HUD_T_SPACE + 2, P1_R,  L"╩");
    mvaddwstr(HUD_T_SPACE + 2, HP_R,  L"╩");
    mvaddwstr(HUD_T_SPACE + 2, GUN_R, L"╩");
    mvaddwstr(HUD_T_SPACE + 2, KIL_R, L"╝");
    hud_fill_hline(HUD_T_SPACE + 2, P1_L,  P1_R);
    hud_fill_hline(HUD_T_SPACE + 2, HP_L,  HP_R);
    hud_fill_hline(HUD_T_SPACE + 2, GUN_L, GUN_R);
    hud_fill_hline(HUD_T_SPACE + 2, KIL_L, KIL_R);
    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
}

/* ══════════════════════════════════════════════════════════════════════════
   CONTROLS — vertical panel, bottom-left
   ══════════════════════════════════════════════════════════════════════════ */

#define KEY_INNER  12   // content width between ║ borders
#define KEY_L_SPACE    2   // 2 column gap from left edge
#define KEY_BOT_SPACE     1   // 1 row gap from bottom edge

void ui_draw_controls(int rows)
{
    typedef struct { const char *key; const char *desc; } Bind;
    static const Bind binds[] = {
        { "W",   "forward" },
        { "S",   "back"    },
        { "A",   "strafe<" },
        { "D",   "strafe>" },
        { "K",   "turn <"  },
        { "L",   "turn >"  },
        { "SPC", "fire"    },
        { "M",   "map"     },
        { "Q",   "quit"    },
    };
    int nbinds = (int)(sizeof(binds) / sizeof(binds[0]));

    // panel sits at bottom-left, exactly tall enough for all binds + 2 border rows
    int panel_h = nbinds + 2;
    int top     = rows - panel_h - KEY_BOT_SPACE;
    int left    = KEY_L_SPACE;
    int right   = KEY_L_SPACE + KEY_INNER + 1;

    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
    mvaddwstr(top, left,  L"╔");
    mvaddwstr(top, right, L"╗");
    {
        const char *title = " KEYS ";
        int tlen = (int)strlen(title);
        int tmid = left + 1 + (KEY_INNER - tlen) / 2;
        for (int i = left + 1; i < right; i++) {
            move(top, i);
            int ti = i - tmid;
            if (ti >= 0 && ti < tlen) {
                wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
                wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
                addch(title[ti]);
                wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
                wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
            } else {
                addwstr(L"═");
            }
        }
    }
    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

    for (int b = 0; b < nbinds; b++) {
        int row = top + 1 + b;

        wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
        mvaddwstr(row, left,  L"║");
        mvaddwstr(row, right, L"║");
        wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);

        for (int i = left + 1; i < right; i++) mvaddch(row, i, ' ');

        wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
        mvprintw(row, left + 1, " %-3s", binds[b].key);
        wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);

        wattron(stdscr, COLOR_PAIR(CP_MAP_BDR));
        mvprintw(row, left + 5, "%-*s", KEY_INNER - 4, binds[b].desc);
        wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR));
    }

    int bot = top + panel_h - 1;
    wattron(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
    mvaddwstr(bot, left,  L"╚");
    mvaddwstr(bot, right, L"╝");
    for (int i = left + 1; i < right; i++)
        mvaddwstr(bot, i, L"═");
    wattroff(stdscr, COLOR_PAIR(CP_MAP_BDR) | A_BOLD);
}