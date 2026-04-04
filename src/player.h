#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int id;
    double x, y;
    double angle;
    int health;
    int col;
    int kills;
    int cur_gun;
    int unlocked_guns;
} Player;


#endif