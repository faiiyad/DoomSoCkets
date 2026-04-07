#ifndef RAYCASTER_MAP_H
#define RAYCASTER_MAP_H

#include "defs.h"

int map_solid(int mx, int my);
int map_type(int mx, int my);
void map_find_spawn(double *x, double *y);
extern const char MAP[MAP_H][MAP_W + 1];

#endif