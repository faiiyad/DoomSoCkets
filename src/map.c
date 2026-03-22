#include "defs.h"
#include "map.h"

const char MAP[MAP_H][MAP_W + 1] = {
    "1111111111111111",
    "1.1..1....11...1",
    "1.1.....11..1..1",
    "1...1.9.......11",
    "111.1...1.1....1",
    "1...1.1.1.1.11.1",
    "1.1...1...1....1",
    "1.1.111.1...11.1",
    "1...1...1.1....1",
    "1.111.1.1.1.1..1",
    "1.....1...1.1..1",
    "1.1.1.1.111....1",
    "1.1.1.....1.1..1",
    "1..............1",
    "1..1...........1",
    "1111111111111111",
};

int map_solid(int mx, int my)
{
    if (mx < 0 || mx >= MAP_W || my < 0 || my >= MAP_H) return 1;
    return MAP[my][mx] != '.' && MAP[my][mx] != '9';
}

int map_type(int mx, int my)
{
    if (mx < 0 || mx >= MAP_W || my < 0 || my >= MAP_H) return 1;
    char c = MAP[my][mx];
    if (c >= '1' && c <= '9') return c - '0';
    return 0;
}

void map_find_spawn(double *x, double *y)
{
    *x = 8.0;
    *y = 8.0;
    for (int my = 0; my < MAP_H; my++)
        for (int mx = 0; mx < MAP_W; mx++)
            if (MAP[my][mx] == '9') {
                *x = mx + 0.5;
                *y = my + 0.5;
            }
}