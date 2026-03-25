#ifndef DEFS_H
#define DEFS_H
#include <math.h>

// ── game constants ────────────────────────────────────────────────────────
#define MAP_W     16
#define MAP_H     16
#define FOV       (M_PI / 4.0)
#define MOVE_SPD  0.08
#define ROT_SPD   0.155
#define MAX_DEPTH 20.0

static const char *SHADES = " .:-=+*#%@";
#define N_SHADES 10

// ── color pair IDs ────────────────────────────────────────────────────────
typedef enum {
    // 3D view
    CP_WALL1 = 1,
    CP_WALL2,
    CP_WALL3,
    CP_WALL4,
    CP_FLOOR,
    CP_CEIL,
    CP_HUD,
    CP_XWALL,

    // minimap
    CP_MAP_BDR,
    CP_MAP_P,
    CP_MAP_EMPTY,
    CP_WALL1_M,
    CP_WALL2_M,
    CP_WALL3_M,
    CP_WALL4_M,

    // gun
    CP_GUN,
    CP_FLASH,
    HAND_CLR,
    HAND_CLR_S,
    GUN_BDR,
    MUZ_1,
    MUZ_2,
    MUZ_3,
    GUN_BODY,
    GUN_TRIM,
    GUN_DIRT,

    // title
    TITLE1,
    TITLE2,
    TITLE3,
    TITLE4,
    TITLEBG,

    CP_SPRITE_R,
    CP_SPRITE_D,
    CP_SPRITE_O,
    CP_SPRITE_W,
    
    CP_COUNT   // total — used to size the table
} ColorPair;

// ── aliases for legacy names in gun.c ─────────────────────────────────────
#define CP_MAP_W  CP_MAP_BDR
#define CP_MAP2   CP_WALL2_M

#endif