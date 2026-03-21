#ifndef RENDER_H
#define RENDER_H

#define MAX_DISTANCE 20.0f
#define FOV 60.0f

// Casts a ray from the player at the given angle
// Sets wallType to the map value of the wall hit (1 or 2), or 0 if nothing hit
// Returns the distance to the wall
double castRay(double angle, int* wallType);

// Renders the full 3D view into ncurses' buffer
void renderView(void);

#endif
