#ifndef RAY_H
#define RAY_H

#include "entity.h"


double cast_ray(double px, double py, double angle,
                int *side, int *wtype);

double cast_ray_to_entity(double px, double py, double angle,
                        const Entity *e, double hitbox, int *hit);

#endif