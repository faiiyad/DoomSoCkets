#include <math.h>
#include "ray.h"
#include "map.h"
#include "entity.h"

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

double cast_ray_to_entity(double px, double py, double angle,
                          const Entity *e, double hitbox, int *hit)
{
    double dx = e->x - px;
    double dy = e->y - py;
    double dist = sqrt(dx*dx + dy*dy);
    double ray_angle = atan2(dy, dx);
    double angle_diff = fmod(ray_angle - angle + M_PI, 2*M_PI) - M_PI;

    if (fabs(angle_diff) >= atan2(hitbox / 2.0, dist)) {
        *hit = 0;
        return INFINITY;
    }

    int side, wtype;
    double wall_dist = cast_ray(px, py, angle, &side, &wtype);
    if (wall_dist < dist) {
        *hit = 0;
        return INFINITY;
    }

    *hit = 1;
    return dist;
}