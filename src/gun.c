#include <math.h>
#include <stdlib.h>
#include <ncurses.h>
#include "gun.h"
#include "defs.h"

int gun_frame = 0;
int gun_timer = 0;
int gun_status = 0;
float gun_offset  = 20.0f;
float gun_offset2 = 20.0f;

// ── gun0 art ───────────────────────────────────────────────────────────────
static const char *gun0_art[] = {
    "   ___",
    "  [ X ]",
    "   {T}",
    " _/#0#\\_",
    "[@\\=---\\\\_",
    "[$@\\=----\\\\",
    "[\\$@\\==---\\\\",
    "\\[\\$@\\==---\\\\_",
    " \\[\\$@\\==----\\\\",
    "  \\[\\$@\\===---\\\\",
    "   \\[\\$@\\_=====\\\\__",
    "    \\[\\$@]\\[\\HHHHH\\]__",
    "      Y$@[/==|[@A@@@A@]__",
    "     /[]\\=\\==[\\[########]/",
    "     \\****[\\==|========|/",
    "      \\****[\\_|@||====|Y\\_",
    "       \\_**_/|@|[########]",
    "        \\@@@@@||==__|V^",
    "        /_/Y$||==__V^",
    "       /_/Y$||==__/",
    "      /_/Y$||==__/",
};

static const char *gun0_clr[] = {
    "   BBB",
    "  F G F",
    "   FFF",
    " FFBFBFF",
    "FEFBBBBFF",
    "FBEFBBBBBF",
    "FFBEFBBBBBF",
    "FFFBEFBBBBBFFF",
    " FFFBEFBBBBBBFF",
    "  FFFBEFBBBBBBFF",
    "   FFFBEFFBBBBBFFF",
    "    FFFBEFFHHHHHHHHHFF",
    "      FBEFFBBFHEGEEEGEHFFF",
    "     FFFFFFBBFFHHHHHHHHHHF",
    "     FDDDDFFBBFFFFFFFFFFF",
    "      FDDDDFFFFEBBBBBBBFFF",
    "       FFDDFFFEBHHHHHHHHHH",
    "        FEEEEEBBBBBBBFF",
    "        FDFFBBBBBBBFF",
    "       FDFFBBBBBBBF",
    "      FDFFBBBBBFFF",
};

// ── gun1 art ───────────────────────────────────────────────────────────────
static const char *gun1_art[] = {
    "   ___",
    "  [ X ]",
    "   {T}",
    "  /#0#\\",
    " |\\=---\\ ",
    " |@\\=---\\",
    " |@[\\==--\\",
    " |@|$\\_==-\\_",
    " |@\\$[\\@\\==\\@\\",
    "  \\@\\$[\\@\\==\\@\\",
    "   \\@\\$[\\@\\==\\@\\ ",
    "    \\@\\|_V[===\\V",
    "     \\@|@\\\\]___]\\__",
    "      \\|_/\\\\==\\}___}_",
    "         |*\\Y==\\}____}_",
    "       _/**/]L==-\\}_____}",
    "      /V*_/$/@L==-\\}_____}",
    "    _/**/$$/\\$@\\==-\\}_____}",
    "   /V**/$/   \\$@\\==-//_|_\\\\",
    "   |**/$/    /L$@\\==\\\\____\\\\",
    "    \\V$/    /L_\\$@\\==V_____V",
    "           /L__/\\$@\\==\\=====\\",
};

static const char *gun1_clr[] = {
    "   BBB",
    "  F G F",
    "   FFF",
    "  FBFBF",
    " FFBBBBF",
    " FEFBBBBF",
    " FEFFBBBBF",
    " FEFFFFBBBFF",
    " FEFFFFEFBBFEF",
    "  FEFFFFEFBBFEF",
    "   FEFFFFEFBBFEF",
    "    FEFFFFFBBBFF",
    "     FEFEFFFFFFFFFF",
    "      FFFFFFBBFHHHHHF",
    "         FDFFBBFHHHHHHF",
    "       FFDDFFFBBBFHHHHHHH",
    "      FFDFFFFEFBBBFHHHHHHH",
    "    FFDDFFFFFFFEFBBBFHHHHHHH",
    "   FFDDFFF   FFEFBBBFFBBBFF",
    "   FDDFFF    FFFEFBBFFBBBBFF",
    "    FFFF    FFBFFEFBBFBBBBBF",
    "           FFBBFFFEFBBFBBBBBF",
};

// ── shared colour resolver ─────────────────────────────────────────────────
static int resolve_color(char c) {
    switch (c) {
        case 'F': return GUN_TRIM;
        case 'B': return GUN_BODY;
        case 'E': return GUN_ACC;
        case 'S': return MUZ_1;
        case 'D': return GUN_DIRT;
        case 'H': return HAND_CLR_S;
        case 'G': return HAND_CLR;
        default:  return GUN_TRIM;
    }
}

// ── draw_gun ───────────────────────────────────────────────────────────────
// gun_id: 0 = gun0, 1 = gun1
void draw_gun(int gun_id, int rows, int cols)
{
    const char **art;
    const char **clr;
    int num_rows;

    if (gun_id == 1) {
        art      = gun1_art;
        clr      = gun1_clr;
        num_rows = (int)(sizeof(gun1_art) / sizeof(gun1_art[0]));
    } else {
        art      = gun0_art;
        clr      = gun0_clr;
        num_rows = (int)(sizeof(gun0_art) / sizeof(gun0_art[0]));
    }

    /* ── slide animation ─────────────────────────────────────── */
    float target2 = (gun_status == 1) ? 0.0f : 20.0f;
    gun_offset  += (0.0f    - gun_offset)  * 0.18f;
    gun_offset2 += (target2 - gun_offset2) * 0.18f;

    int kick      = (gun_frame == 1) ? 1 : 0;
    int base      = rows + kick;
    int cx        = cols - 55;
    int mirror_cx = 55;
    int r         = base - 22 + (int)gun_offset;
    int r2        = base - 20 + (int)gun_offset2;

    /* ── muzzle flash — right gun ────────────────────────────── */
    if (gun_frame == 1) {
        int cy  = r + 2;
        int cx0 = cx + 4;
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -7; dx <= 7; dx++) {
                float dist = sqrtf((dx*0.55f)*(dx*0.55f) + (float)(dy*dy));
                char ch; int pair;
                if      (dist < 2.0f)                          continue;
                else if (dist < 3.0f) { ch = 'O'; pair = MUZ_1; }
                else if (dist < 3.8f) { ch = 'o'; pair = MUZ_1; }
                else if (dist < 4.3f) { ch = '*'; pair = MUZ_2; }
                else if (dist < 4.8f && (rand()%3 != 0))
                                      { ch = '.'; pair = MUZ_3; }
                else continue;
                if (rand()%3 == 0) ch = ' ';
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(cy + dy, cx0 + dx, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }

    /* ── muzzle flash — left gun ─────────────────────────────── */
    if (gun_status == 1 && gun_frame == 1) {
        int cy  = r2 + 2;
        int cx0 = mirror_cx - 4;
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -7; dx <= 7; dx++) {
                float dist = sqrtf((dx*0.55f)*(dx*0.55f) + (float)(dy*dy));
                char ch; int pair;
                if      (dist < 2.0f)                          continue;
                else if (dist < 3.0f) { ch = 'O'; pair = MUZ_1; }
                else if (dist < 3.8f) { ch = 'o'; pair = MUZ_1; }
                else if (dist < 4.3f) { ch = '*'; pair = MUZ_2; }
                else if (dist < 4.8f && (rand()%3 != 0))
                                      { ch = '.'; pair = MUZ_3; }
                else continue;
                if (rand()%3 == 0) ch = ' ';
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(cy + dy, cx0 + dx, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }

    /* ── right gun ───────────────────────────────────────────── */
    for (int row = 0; row < num_rows; row++) {
        const char *a = art[row];
        const char *c = clr[row];
        int ci = 0;
        for (int col = 0; a[col] != '\0'; col++) {
            if (a[col] == ' ') { ci++; continue; }
            int pair = resolve_color(c[ci]);
            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(r + row, cx + col, a[col]);
            attroff(COLOR_PAIR(pair) | A_BOLD);
            ci++;
        }
    }

    /* ── left (mirrored) gun ─────────────────────────────────── */
    if (gun_offset2 < 19.5f) {
        for (int row = 0; row < num_rows; row++) {
            const char *a = art[row];
            const char *c = clr[row];
            int art_len = 0;
            while (a[art_len] != '\0') art_len++;
            int ci = 0;
            for (int col = 0; col < art_len; col++) {
                if (a[col] == ' ') { ci++; continue; }
                int pair = resolve_color(c[ci]);
                int draw_col = mirror_cx - col;
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(r2 + row, draw_col, a[col]);
                attroff(COLOR_PAIR(pair) | A_BOLD);
                ci++;
            }
        }
    }
}