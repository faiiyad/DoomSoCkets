#ifndef RAYCASTER_MAP_H
#define RAYCASTER_MAP_H

#include "defs.h"

/* Returns 1 if the cell blocks movement */
int map_solid(int mx, int my);

/* Returns the numeric tile type (1-9), or 0 for open space */
int map_type(int mx, int my);

/* Writes the centre of the '9' spawn tile into *x and *y */
void map_find_spawn(double *x, double *y);

#endif /* RAYCASTER_MAP_H */