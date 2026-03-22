#include <math.h>
#include "ray.h"
#include "map.h"

double cast_ray(double px, double py, double angle,
                int *side, int *wtype)
{
    double dx = cos(angle);
    double dy = sin(angle);

    int mx = (int)px, my = (int)py;

    double ddx = fabs(1.0 / (dx == 0 ? 1e-30 : dx));
    double ddy = fabs(1.0 / (dy == 0 ? 1e-30 : dy));

    double sdx, sdy;
    int stepx, stepy;

    if (dx < 0) { stepx = -1; sdx = (px - mx) * ddx; }
    else         { stepx =  1; sdx = (mx + 1.0 - px) * ddx; }
    if (dy < 0) { stepy = -1; sdy = (py - my) * ddy; }
    else         { stepy =  1; sdy = (my + 1.0 - py) * ddy; }

    int hit = 0;
    *side = 0;
    while (!hit) {
        if (sdx < sdy) { sdx += ddx; mx += stepx; *side = 0; }
        else            { sdy += ddy; my += stepy; *side = 1; }
        if (map_solid(mx, my)) hit = 1;
    }

    *wtype = map_type(mx, my);
    double dist = (*side == 0)
        ? (mx - px + (1 - stepx) / 2.0) / dx
        : (my - py + (1 - stepy) / 2.0) / dy;
    return fabs(dist);
}