#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "client.h"
#include "ui.h"
#include "defs.h"
#include "map.h"
#include "entity.h"

/* ══════════════════════════════════════════════════════════════════════════
   SHARED BORDER PRIMITIVES
   All panel drawing goes through these four functions.
   ══════════════════════════════════════════════════════════════════════════ */

// Fill l..r with ═, then stamp left and right corner chars over it.
// Used for top/bottom borders and dividers.
static void border_hline(int y, int l, int r,
                          const wchar_t *lc, const wchar_t *rc)
{
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    for (int i = l; i <= r; i++) mvaddwstr(y, i, L"═");
    mvaddwstr(y, l, lc);
    mvaddwstr(y, r, rc);
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
}

// Same as border_hline but stamps a centered title in CP_MAP_P over the fill.
// Pass NULL title for a plain ═ line.
static void border_hline_title(int y, int l, int r,
                                const wchar_t *lc, const wchar_t *rc,
                                const char *title)
{
    border_hline(y, l, r, lc, rc);
    if (!title || !title[0]) return;
    int tlen = (int)strlen(title);
    int tmid = l + 1 + (r - l - 1 - tlen) / 2;
    wattron(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
    for (int i = 0; i < tlen; i++)
        mvaddch(y, tmid + i, title[i]);
    wattroff(stdscr, COLOR_PAIR(CP_MAP_P) | A_BOLD);
}

// ║ borders on left and right, spaces in between.
static void border_row(int y, int l, int r)
{
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    mvaddwstr(y, l, L"║");
    mvaddwstr(y, r, L"║");
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    for (int i = l + 1; i < r; i++) mvaddch(y, i, ' ');
}

// Convenience macros for the three common border line types
#define panel_top(y,l,r,title) border_hline_title(y, l, r, L"╔", L"╗", title)
#define panel_bot(y,l,r)       border_hline(y, l, r, L"╚", L"╝")
#define panel_div(y,l,r)       border_hline(y, l, r, L"╠", L"╣")

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

    border_hline_title(top - 1, left, right, L"╔", L"╗", "[RADAR]");
    border_hline(top, left, right, L"╠", L"╣");

    wattron(stdscr, COLOR_PAIR(CP_UI_BDR));
    for (int i = 0; i < MAP_H; i++) {
        mvaddwstr(oy + i, left,  L"║");
        mvaddwstr(oy + i, right, L"║");
    }
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR));

    // bottom with coordinate leg
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    for (int i = left; i <= right; i++) mvaddwstr(bot, i, L"═");
    mvaddwstr(bot, left,     L"╚");
    mvaddwstr(bot, leg_left, L"╦");
    mvaddwstr(bot, right,    L"╣");
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);

    border_row(bot + 1, leg_left, right);
    move(bot + 1, leg_left + 1);
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR));
    for (int i = 0; i < clen; i++) addch(coords[i]);
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR));

    panel_bot(leg_bot, leg_left, right);
}

static void draw_map_tiles(int ox, int oy, int rows, int cols)
{
    for (int my = 0; my < MAP_H; my++) {
        for (int mx = 0; mx < MAP_W; mx++) {
            int sy = oy + my;
            int sx = ox + mx * 2;
            if (sy >= rows - 1 || sx + 1 >= cols) continue;
            if (map_solid(mx, my)) {
                // int t  = map_type(mx, my);
                int cp = CP_WALL_M;
                wattron(stdscr, COLOR_PAIR(cp) | A_BOLD);
                mvwaddstr(stdscr, sy, sx, "░░");
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
            wattron(stdscr, COLOR_PAIR(e->col) | A_BOLD);
            mvwaddch(stdscr, ey, ex, 'X');
            wattroff(stdscr, COLOR_PAIR(e->col) | A_BOLD);
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
    int ox = cols - (MAP_W * 2 + 2) - 1;
    if (ox < 0) ox = 0;
    draw_map_border(ox, 3, p);
    draw_map_tiles (ox, 3, rows, cols);
    draw_map_player(p,  ox, 3, rows, cols);
}

/* ══════════════════════════════════════════════════════════════════════════
   HUD — horizontal stats bar, top of screen
   ══════════════════════════════════════════════════════════════════════════ */

#define HUD_L_SPACE  2
#define HUD_T_SPACE  1
#define P_INNER      4
#define HP_INNER     16
#define GUN_INNER    13
#define KIL_INNER    9
#define BAR_W        10

#define PLAYER_L  HUD_L_SPACE
#define PLAYER_R  (PLAYER_L + P_INNER   + 1)
#define HP_L      PLAYER_R
#define HP_R      (HP_L     + HP_INNER  + 1)
#define GUN_L     HP_R
#define GUN_R     (GUN_L    + GUN_INNER + 1)
#define KIL_L     GUN_R
#define KIL_R     (KIL_L    + KIL_INNER + 1)

void ui_draw_hud(Player *p)
{
    int hp     = p->health;
    int max_hp = 100;
    if (hp < 0)      hp = 0;
    if (hp > max_hp) hp = max_hp;

    // top border — full ═ fill, then joints, then section titles on top
    border_hline_title(HUD_T_SPACE, PLAYER_L, KIL_R, L"╔", L"╗", NULL);
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    mvaddwstr(HUD_T_SPACE, PLAYER_R, L"╦");
    mvaddwstr(HUD_T_SPACE, HP_R,     L"╦");
    mvaddwstr(HUD_T_SPACE, GUN_R,    L"╦");
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    border_hline_title(HUD_T_SPACE, HP_L,  HP_R,  L"╦", L"╦", "[HP]");
    border_hline_title(HUD_T_SPACE, GUN_L, GUN_R, L"╦", L"╦", "[GUN]");
    border_hline_title(HUD_T_SPACE, KIL_L, KIL_R, L"╦", L"╗", "[KILLS]");

    // content row — outer borders then internal joints
    border_row(HUD_T_SPACE + 1, PLAYER_L, KIL_R);
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    mvaddwstr(HUD_T_SPACE + 1, PLAYER_R, L"║");
    mvaddwstr(HUD_T_SPACE + 1, HP_R,     L"║");
    mvaddwstr(HUD_T_SPACE + 1, GUN_R,    L"║");
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);

    // P1 label
    {
        int lcol = PLAYER_L + 1 + (P_INNER - 2) / 2;
        wattron(stdscr, COLOR_PAIR(CP_UI_LABEL) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, lcol, "P1");
        wattroff(stdscr, COLOR_PAIR(CP_UI_LABEL) | A_BOLD);
    }

    // HP bar
    {
        int filled   = (BAR_W * hp) / max_hp;
        int bar_pair = (hp > max_hp*2/3) ? CP_UI_GOOD
                     : (hp > max_hp/3)   ? CP_UI_WARN
                     : CP_UI_BAD;
        int col = HP_L + 1;
        mvaddch(HUD_T_SPACE + 1, col++, '[');
        wattron(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
        for (int i = 0; i < filled;  i++) mvaddwstr(HUD_T_SPACE + 1, col++, L"█");
        wattroff(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
        wattron(stdscr, COLOR_PAIR(CP_UI_BDR));
        for (int i = filled; i < BAR_W; i++) mvaddwstr(HUD_T_SPACE + 1, col++, L"░");
        wattroff(stdscr, COLOR_PAIR(CP_UI_BDR));
        mvaddch(HUD_T_SPACE + 1, col++, ']');
        wattron(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, col, " %d", hp);
        wattroff(stdscr, COLOR_PAIR(bar_pair) | A_BOLD);
    }

    // GUN cell
    wattron(stdscr, COLOR_PAIR(CP_UI_TEXT) | A_BOLD);
    mvprintw(HUD_T_SPACE + 1, GUN_L + 1, " %-6s %2ddmg", "M16", 12);
    wattroff(stdscr, COLOR_PAIR(CP_UI_TEXT) | A_BOLD);

    // KILLS cell
    {
        char kbuf[8];
        snprintf(kbuf, sizeof(kbuf), "%d", client_get_kills());
        int kcol = KIL_L + 1 + (KIL_INNER - (int)strlen(kbuf)) / 2;
        wattron(stdscr, COLOR_PAIR(CP_UI_TEXT) | A_BOLD);
        mvprintw(HUD_T_SPACE + 1, kcol, "%s", kbuf);
        wattroff(stdscr, COLOR_PAIR(CP_UI_TEXT) | A_BOLD);
    }

    // bottom border — same pattern as top
    border_hline(HUD_T_SPACE + 2, PLAYER_L, KIL_R, L"╚", L"╝");
    wattron(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
    mvaddwstr(HUD_T_SPACE + 2, PLAYER_R, L"╩");
    mvaddwstr(HUD_T_SPACE + 2, HP_R,     L"╩");
    mvaddwstr(HUD_T_SPACE + 2, GUN_R,    L"╩");
    wattroff(stdscr, COLOR_PAIR(CP_UI_BDR) | A_BOLD);
}

/* ══════════════════════════════════════════════════════════════════════════
   CONTROLS — vertical panel, bottom-left
   ══════════════════════════════════════════════════════════════════════════ */

#define KEY_INNER    12
#define KEY_L_SPACE   2
#define KEY_B_SPACE   1
#define KEY_NBINDS    9   // also used by server panel to locate keys_top

void ui_draw_controls(int rows)
{
    static const struct { const char *key, *desc; } binds[KEY_NBINDS] = {
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

    int top   = rows - (KEY_NBINDS + 2) - KEY_B_SPACE;
    int left  = KEY_L_SPACE;
    int right = KEY_L_SPACE + KEY_INNER + 1;

    panel_top(top, left, right, "[KEYS]");

    for (int b = 0; b < KEY_NBINDS; b++) {
        int row = top + 1 + b;
        border_row(row, left, right);
        wattron(stdscr, COLOR_PAIR(CP_UI_LABEL) | A_BOLD);
        mvprintw(row, left + 1, " %-3s", binds[b].key);
        wattroff(stdscr, COLOR_PAIR(CP_UI_LABEL) | A_BOLD);
        wattron(stdscr, COLOR_PAIR(CP_UI_TEXT));
        mvprintw(row, left + 5, "%-*s", KEY_INNER - 4, binds[b].desc);
        wattroff(stdscr, COLOR_PAIR(CP_UI_TEXT));
    }

    panel_bot(top + KEY_NBINDS + 1, left, right);
}

/* ══════════════════════════════════════════════════════════════════════════
   SERVER PANEL — left side, between HUD and keys
   ══════════════════════════════════════════════════════════════════════════ */

static int srv_connected = 0;

void ui_toggle_connect(void) { srv_connected = !srv_connected; }

#define SRV_INNER  20
#define SRV_LEFT   KEY_L_SPACE

void ui_draw_server(int rows)
{
    int keys_top = rows - (KEY_NBINDS + 2) - KEY_B_SPACE;
    int top      = HUD_T_SPACE + 3 + 1;
    int bot      = keys_top - 2;
    int l        = SRV_LEFT;
    int r        = SRV_LEFT + SRV_INNER + 1;

    if (bot - top < 3) return;

    panel_top(top, l, r, "[SERVER]");

    border_row(top + 1, l, r);
    if (srv_connected) {
        wattron(stdscr, COLOR_PAIR(CP_UI_GOOD) | A_BOLD);
        mvprintw(top + 1, l + 2, "● CONNECTED C=leave");
        wattroff(stdscr, COLOR_PAIR(CP_UI_GOOD) | A_BOLD);
    } else {
        wattron(stdscr, COLOR_PAIR(CP_UI_BAD) | A_BOLD);
        mvprintw(top + 1, l + 2, "○ OFFLINE   C=join ");
        wattroff(stdscr, COLOR_PAIR(CP_UI_BAD) | A_BOLD);
    }

    panel_div(top + 2, l, r);

    int content_rows = bot - (top + 2) - 1;
    int drawn = 0;

    // header
    if (drawn < content_rows) {
        border_row(top + 3, l, r);
        wattron(stdscr, COLOR_PAIR(CP_UI_TEXT));
        mvprintw(top + 3, l + 2, "%-3s %-10s %-4s", "ID", "HEALTH", "KIL");
        wattroff(stdscr, COLOR_PAIR(CP_UI_TEXT));
        drawn++;
    }

    for (int i = 0; i < num_entities && drawn < content_rows; i++) {
        Entity *e = &entities[i];
        int row = top + 3 + drawn;
        border_row(row, l, r);

        wattron(stdscr, COLOR_PAIR(CP_UI_LABEL) | A_BOLD);
        mvprintw(row, l + 2, "P%-2d", e->id);
        wattroff(stdscr, COLOR_PAIR(CP_UI_LABEL) | A_BOLD);

        int hp      = e->health;
        int bar_max = 10;
        int filled  = (bar_max * hp) / 100;
        if (filled < 0) filled = 0;
        if (filled > bar_max) filled = bar_max;
        int hp_pair = (hp > 66) ? CP_UI_GOOD : (hp > 33) ? CP_UI_WARN : CP_UI_BAD;
        wattron(stdscr, COLOR_PAIR(hp_pair) | A_BOLD);
        for (int b = 0; b < filled;   b++) mvaddwstr(row, l + 6 + b, L"█");
        wattroff(stdscr, COLOR_PAIR(hp_pair) | A_BOLD);
        wattron(stdscr, COLOR_PAIR(CP_UI_BDR));
        for (int b = filled; b < bar_max; b++) mvaddwstr(row, l + 6 + b, L"░");
        mvprintw(row, l + 17, "%-3d", 10);
        wattroff(stdscr, COLOR_PAIR(CP_UI_BDR));

        drawn++;
    }

    while (drawn < content_rows) {
        border_row(top + 3 + drawn, l, r);
        drawn++;
    }

    panel_bot(bot, l, r);
}

/* ══════════════════════════════════════════════════════════════════════════
   EVENT LOG — bottom-right, below minimap
   ══════════════════════════════════════════════════════════════════════════ */

#define LOG_MAX     32
#define LOG_INNER   (MAP_W * 2 + 2)/2
#define LOG_HEIGHT  17
#define LOG_BOT     1

static char log_lines[LOG_MAX][64];
static int  log_count = 0;

void ui_log_event(const char *msg)
{
    if (log_count < LOG_MAX) {
        snprintf(log_lines[log_count++], 64, "%s", msg);
    } else {
        memmove(log_lines[0], log_lines[1], sizeof(log_lines[0]) * (LOG_MAX - 1));
        snprintf(log_lines[LOG_MAX - 1], 64, "%s", msg);
    }
}

void ui_draw_eventlog(int rows, int cols)
{
    int r   = cols - 3;
    int l   = r - LOG_INNER - 1;
    int bot = rows - 1 - LOG_BOT;
    int top = bot - LOG_HEIGHT - 1;

    if (l < 0 || top < 0) return;

    panel_top(top, l, r, "[EVENTS]");

    int start = log_count - LOG_HEIGHT;
    if (start < 0) start = 0;

    for (int row = 0; row < LOG_HEIGHT; row++) {
        int y  = top + 1 + row;
        int li = start + row;
        border_row(y, l, r);
        if (li < log_count) {
            wattron(stdscr, COLOR_PAIR(CP_UI_BDR));
            mvprintw(y, l + 2, "%-*.*s", LOG_INNER - 2, LOG_INNER - 2, log_lines[li]);
            wattroff(stdscr, COLOR_PAIR(CP_UI_BDR));
        }
    }

    panel_bot(bot, l, r);
}