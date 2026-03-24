#include <math.h>
#include <stdlib.h>
#include <ncurses.h>
#include "gun.h"
#include "defs.h"

int gun_frame = 0;
int gun_timer = 0;

// ── gun art ────────────────────────────────────────────────────────────────
// Each string is one row of the gun drawn left to right.
// Spaces are skipped (transparent), everything else is drawn.
static const char *gun_art[] = {
    "    ___",                        // r+0
    "   [ X ]",                       // r+1
    "    {T}",                         // r+2
    "  _/#0#\\_",                     // r+3
    " [\@\\=---\\_",                  // r+4
    " [\$@\\=----\\",                 // r+5
    " [\\$@\\==---\\",                // r+6
    " \\[\\$@\\==---\\_",             // r+7
    "  \\[\\$@\\==----\\_",           // r+8
    "   \\[\\$@\\==----\\",           // r+9
    "    \\[\\$@]\\[HHHHH]__",        // r+10
    "      |$@[\\=|[@A@@@A@]|__",     // r+11
    "      //=[\\=_[########]/",      // r+12
    "     |[*****[\\_|====| |/",      // r+13
    "      \\******/|@**___Y__",      // r+14
    "       \\_***/|@**[#######]",    // r+15
    "         \\/@@@________V",       // r+16
    "         /_/$$||==__V",          // r+17
    "        /Y$$||==___/",           // r+18
    "       /Y$$|==____/",            // r+19
};

// ── colour map ─────────────────────────────────────────────────────────────
// Parallel to gun_art — must be the same length per row as gun_art.
// Color code letters:
//   F = GUN_TRIM   (dark green   — frame / outline)
//   B = GUN_BODY   (olive green  — barrel body / inner edges)
//   E = CP_WALL2   (cyan         — energy cells $ @)
//   S = MUZ_1      (yellow       — scope X)
//   D = GUN_DIRT   (tan/khaki    — worn decal *)
//   H = HAND_CLR_S (muted red    — grip highlight H)
//   G = HAND_CLR   (dark red     — grip fill)
//   Space = must match spaces in gun_art (transparent)
static const char *gun_clr[] = {
    "    FFF",                         // r+0   ___
    "   F G F",                        // r+1   [ X ]
    "    FDF",                         // r+2   {T}
    "  FFBEBFF",                       // r+3   _/#0#\_
    " FFEBBBBFB",                      // r+4   [\@\=--\_
    " FFEEBBBBBBF",                    // r+5   [\$@\=---
    " FFFEEBBBBBBF",                   // r+6   [\$@\==---
    " FFFFEEBBBBBBFB",                 // r+7   \[\$@\==---\_
    "  FFFFFEEBBBBBBFB",               // r+8   \[\$@\==----\_
    "   FFFFFFEEBBBBBBBF",             // r+9   \[\$@\==----
    "    FFEEFFFHHHHHHHF",             // r+10  \[\$@]\[HHHHH]__
    "      FEEFBBEEEEFEFFF",           // r+11  |$@[\=|[@A@@@A@]|__
    "      BBBFBBBFBBBBBBBBFF",        // r+12  //=[\=_[########]/
    "     FFDDDDDFBBBBBBFFFFB",        // r+13  |[*****[\_|====| |/
    "      FDDDDDDFFEBBBBFFF",         // r+14  \ **** / @  ___Y__
    "       FFDDDFFEDDDBBBBBBBF",      // r+15  \_**_/|@**[#######]
    "         FFEEEFFFFFFFFFBF",       // r+16  \@@@@________V
    "         FFFGGGGBBBBF",           // r+17  /_/$$||==__V
    "        FFGGGGBBBBBBF",           // r+18  /Y$$||==___/
    "       FFGGGBBBBBBBBBF",          // r+19  /Y$$|==____/
};

static int get_color(char c) {
    switch (c) {
        case 'F': return GUN_TRIM;
        case 'B': return GUN_BODY;
        case 'E': return CP_WALL2;
        case 'S': return MUZ_1;
        case 'D': return GUN_DIRT;
        case 'H': return HAND_CLR_S;
        case 'G': return HAND_CLR;
        default:  return GUN_TRIM;
    }
}

void draw_gun(int rows, int cols)
{
    int kick = (gun_frame == 1) ? 1 : 0;
    int base = rows + kick;
    int cx   = cols - 65;
    int r    = base - 20;

    // ── muzzle flash ──────────────────────────────────────────────────────
    if (gun_frame == 1) {
        int cy  = r - 1;
        int cx0 = cx + 4;
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -7; dx <= 7; dx++) {
                float dist = sqrtf((dx * 0.55f) * (dx * 0.55f) + (float)(dy * dy));
                char ch; int pair;
                if      (dist < 2.0f)                        continue;
                else if (dist < 3.0f) { ch = 'O'; pair = MUZ_1; }
                else if (dist < 3.8f) { ch = 'o'; pair = MUZ_1; }
                else if (dist < 4.3f) { ch = '*'; pair = MUZ_2; }
                else if (dist < 4.8f && (rand() % 3 != 0))
                                      { ch = '.'; pair = MUZ_3; }
                else continue;
                if (rand() % 3 == 0) ch = ' ';
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(cy + dy, cx0 + dx, ch);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }
    }

    // ── draw gun from arrays ──────────────────────────────────────────────
    // Walk gun_art and gun_clr in lockstep.
    // art[col] gives the character to draw.
    // clr[col] gives the color code for that character.
    // Spaces in art are transparent — skip them.
    int num_rows = sizeof(gun_art) / sizeof(gun_art[0]);
    for (int row = 0; row < num_rows; row++) {
        const char *art = gun_art[row];
        const char *clr = gun_clr[row];

        for (int col = 0; art[col] != '\0' && clr[col] != '\0'; col++) {
            char ac = art[col];
            if (ac == ' ') continue;

            int pair = get_color(clr[col]);
            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(r + row, cx + col, ac);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }
}