#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int id;
    double x, y;
    double angle;
    int health;
    int col;
    int cur_gun;
    int unlocked_guns;
    int kill_count;
} Player;


#endif