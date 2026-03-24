#ifndef DEFS_H
#define DEFS_H
#include <math.h>

#define MAP_W    16
#define MAP_H    16
#define FOV      (M_PI / 4.0)
#define MOVE_SPD 0.08
#define ROT_SPD  0.155
#define MAX_DEPTH 20.0

static const char *SHADES = " .:-=+*#%@";
#define N_SHADES 10

// ── 3D view ───────────────────────────────────────────────────────────────
#define CP_WALL1    1
#define CP_WALL2    2
#define CP_WALL3    3
#define CP_WALL4    4
#define CP_FLOOR    5
#define CP_CEIL     6
#define CP_HUD      7
#define CP_XWALL    8

// ── minimap border / UI ───────────────────────────────────────────────────
#define CP_MAP_BDR    9    // border lines     — green on black
#define CP_MAP_P      10   // player arrow     — bright green on black
#define CP_MAP_EMPTY  11   // empty floor      — dark green on dark green (solid)
#define CP_WALL1_M    12   // wall type 1 map  — solid block
#define CP_WALL2_M    13   // wall type 2 map  — solid block
#define CP_WALL3_M    14   // wall type 3 map  — solid block
#define CP_WALL4_M    15   // wall type 4 map  — solid block

// ── gun ───────────────────────────────────────────────────────────────────
#define CP_GUN      16
#define CP_FLASH    17
#define HAND_CLR    18
#define HAND_CLR_S  19
#define GUN_BDR     20
#define MUZ_1       21
#define MUZ_2       22
#define MUZ_3       23
#define GUN_BODY    24
#define GUN_TRIM    25
#define GUN_DIRT    26
// ── used by gun.c for some reason ────────────────────────────
#define CP_MAP_W    CP_MAP_BDR
#define CP_MAP2     CP_WALL2_M

#define TITLE1 27
#define TITLE2 28
#define TITLE3 29
#define TITLE4 30
#define TITLEBG 31

#endif