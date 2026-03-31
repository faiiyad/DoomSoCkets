#ifndef ENTITY_H
#define ENTITY_H

#include "player.h"

#define MAX_ENTITIES  32
#define SPR_W  14
#define SPR_H 16

typedef struct {
    double     x, y;
    double     angle;
    double     dist;
    int        health;
    int        active;
    double        smooth_h;
} Entity;

extern Entity entities[MAX_ENTITIES];
extern int   num_entities;

void entitys_init(double spawn_x, double spawn_y);
void entitys_update(Player *p, int input);
void entitys_draw(Player *p, double *z_buf, int rows, int cols);

#endif