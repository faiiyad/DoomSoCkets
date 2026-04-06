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
    int      kills; 
} Entity;

extern Entity entities[MAX_ENTITIES];
extern int   num_entities;
extern const char **sprite_dir[5];

int col_from_char(char c);
int entity_color(Entity *e, char c);
void entities_init(double spawn_x, double spawn_y);
void entities_update(Player *p, int input);
void entities_draw(Player *p, double *z_buf, int rows, int cols);
void entity_upsert(int id, char col, double x, double y, double angle, int health, int kills);
void entity_upsert_kill(int killer_id);
void entity_remove(int id);

#endif