#ifndef ENTITY_H
#define ENTITY_H

#include "player.h"

#define MAX_ENTITIES  32
#define SPR_W  14
#define SPR_H 16

typedef struct {
    int id;
    double     x, y;
    double     angle;
    double     dist;
    int        health;
    int       col;
} Entity;

extern Entity entities[MAX_ENTITIES];
extern int   num_entities;

void entities_init(double spawn_x, double spawn_y);
void entities_update(Player *p, int input);
void entities_draw(Player *p, double *z_buf, int rows, int cols);
void entity_upsert(int id, double x, double y, double angle, int health);

#endif