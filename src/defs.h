#ifndef DEFS_H
#define DEFS_H
#include <math.h>
#define MAP_W   16
#define MAP_H   16


#define FOV      (M_PI / 4.0)
#define MOVE_SPD 0.08
#define ROT_SPD  0.155
#define MAX_DEPTH 20.0


static const char *SHADES = " .:-=+*#%@";
#define N_SHADES 10


#define CP_WALL1   1
#define CP_WALL2   2
#define CP_WALL3   3
#define CP_WALL4   4
#define CP_FLOOR   5
#define CP_CEIL    6
#define CP_HUD     7
#define CP_MAP_W   8
#define CP_MAP_P   9
#define CP_XWALL  10
#define CP_GUN    11
#define CP_FLASH  12
#define HAND_CLR  13
#define HAND_CLR_S 14
#define GUN_BDR   15
#define MUZ_1     16
#define MUZ_2     17
#define MUZ_3     18

#define GUN_BODY 20
#define GUN_TRIM 21
#define GUN_DIRT 22

#endif