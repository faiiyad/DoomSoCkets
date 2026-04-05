#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "defs.h"
#include "render.h"

static int hit_timer = 0;

void trigger_hit_indicator(void)
{   
    hit_timer = 12;  // frames to show
}

void draw_hit_indicator(int rows, int cols)
{
    if (hit_timer <= 0) return;

    float intensity = (float)hit_timer / 12.0f;
    int depth = (int)(8 * intensity);  // how far inward the splatter goes

    // seed with a stable value so pattern doesn't flicker each frame
    unsigned int seed = 42;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // distance from each edge
            int d_top    = y;
            int d_bottom = rows - y;
            int d_left   = x;
            int d_right  = cols - 1 - x;
            int d_edge   = d_top < d_bottom ? d_top : d_bottom;
            if (d_left  < d_edge) d_edge = d_left;
            if (d_right < d_edge) d_edge = d_right;

            if (d_edge >= depth) continue;

            // pseudo-random splatter — stable per cell
            seed = seed * 1664525u + (unsigned)(y * 1000 + x);
            int r = seed % 100;

            // closer to edge = more likely to draw
            int threshold = (int)(90 * (1.0f - (float)d_edge / depth));
            if (r > threshold) continue;

            // pick char based on distance
            char ch;
            if (d_edge == 0)      ch = '#';
            else if (d_edge <= 2) ch = (r % 3 == 0) ? '*' : '%';
            else if (d_edge <= 5) ch = (r % 2 == 0) ? '.' : ',';
            else                  ch = '.';

            int pair = (d_edge <= depth / 3) ? CP_BLOOD_1 : CP_BLOOD_2;

            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(y, x, ch);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }

    hit_timer--;
}

void draw_hit_indicator_loop(int rows, int cols)
{
    static int tick = 0;
    tick++;

    // oscillate depth in and out using sin
    float intensity = 0.5f + 0.5f * sinf(tick * 0.15f);
    int depth = 4 + (int)(2 * intensity);  // oscillates between 4 and 10

    unsigned int seed = 42;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (y == rows - 1 && x == cols - 1) continue;

            int d_top    = y;
            int d_bottom = rows - 1 - y;
            int d_left   = x;
            int d_right  = cols - 1 - x;
            int d_edge   = d_top < d_bottom ? d_top : d_bottom;
            if (d_left  < d_edge) d_edge = d_left;
            if (d_right < d_edge) d_edge = d_right;

            if (d_edge >= depth) continue;

            seed = seed * 1664525u + (unsigned)(y * 1000 + x);
            int r = seed % 100;

            int threshold = (int)(90 * (1.0f - (float)d_edge / depth));
            if (r > threshold) continue;

            char ch;
            if (d_edge == 0)      ch = '#';
            else if (d_edge <= 2) ch = (r % 3 == 0) ? '*' : '%';
            else if (d_edge <= 5) ch = (r % 2 == 0) ? '.' : ',';
            else                  ch = '.';

            int pair = (d_edge <= depth / 2) ? CP_BLOOD_1 : CP_BLOOD_2;
            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(y, x, ch);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }
}

