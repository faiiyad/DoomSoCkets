#ifndef SPRITE_H
#define SPRITE_H

#include "player.h"

#define MAX_SPRITES  32
#define SPR_W 6  
#define SPR_H 9

typedef enum {
    STATE_IDLE,
    STATE_CHASE,
    STATE_DEAD
} EnemyState;

typedef struct {
    double     x, y;
    double     angle;
    double     dist;
    int        health;
    EnemyState state;
    int        anim_frame;
    int        anim_timer;
    int        active;
} Enemy;

extern Enemy enemies[MAX_SPRITES];
extern int   num_enemies;

void sprites_init(double spawn_x, double spawn_y);
void sprites_update(Player *p, int input);
void sprites_draw(Player *p, double *z_buf, int rows, int cols);

#endif