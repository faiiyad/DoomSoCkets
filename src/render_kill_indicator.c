#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <string.h>
#include "defs.h"
#include "render.h"


static const char *FACE_ART[] = {
    "   ,=======,  ",
    "  //'''''''\\\\",
    " ||'[]'''[]'||",
    " \\\\''''^''''//",
    "   ]'/|||\\'[",
    "   ['|||||']",
    "    =======   ",
};

#define FACE_LINES 7
#define FACE_WIDTH 24

static int face_timer = 0;
static int offset = 1;

void trigger_face_glow(int krab)
{   
    offset = krab;
    face_timer = 20;
}

void draw_face(int rows, int cols)
{
    if (face_timer <= 0) return;

    float intensity = (float)face_timer / 12.0f;

    // pick color based on intensity
    int pair;
    if (intensity > 0.75f)      pair = CP_FACE_1;  // brightest
    else if (intensity > 0.5f)  pair = CP_FACE_2;
    else if (intensity > 0.25f) pair = CP_FACE_3;
    else                        pair = CP_FACE_4;  // dimmest

    int x0 = cols / 2 - FACE_WIDTH / 2 + 5;
    int y0 = rows - FACE_LINES - (offset * FACE_LINES);

    for (int i = 0; i < FACE_LINES; i++) {
        int draw_row = y0 + i;
        if (draw_row < 0 || draw_row >= rows) continue;

        const char *art = FACE_ART[i];
        for (int j = 0; art[j] != '\0'; j++) {
            if (art[j] == ' ') continue;
            int draw_col = x0 + j;
            if (draw_col < 0 || draw_col >= cols) continue;

            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(draw_row, draw_col, art[j]);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }

    face_timer--;
}