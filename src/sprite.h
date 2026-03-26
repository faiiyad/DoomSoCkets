#ifndef SPRITE_H
#define SPRITE_H

#include "player.h"

#define MAX_SPRITES  32
#define SPR_W  14
#define SPR_H 16

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
    double        smooth_h;
} Enemy;

extern Enemy enemies[MAX_SPRITES];
extern int   num_enemies;

void sprites_init(double spawn_x, double spawn_y);
void sprites_update(Player *p, int input);
void sprites_draw(Player *p, double *z_buf, int rows, int cols);

#endif