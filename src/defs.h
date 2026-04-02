#ifndef DEFS_H
#define DEFS_H
#include <math.h>
// ── game constants ────────────────────────────────────────────────────────
#define MAP_W     16
#define MAP_H     16
#define FOV       (M_PI / 4.0)
#define MOVE_SPD  0.08
#define ROT_SPD   0.155
#define MAX_DEPTH 35.0
// ── color pair IDs ────────────────────────────────────────────────────────
#define N_WALL_SHADES 8
#define WALL_COLOR_BASE 180
typedef enum {
    // 3D view-old
    CP_WALL1 = 1,
    CP_XWALL,

    // 3D view — front face shades (bright → dark)
    CP_WALL_F1,   // brightest
    CP_WALL_F2,
    CP_WALL_F3,
    CP_WALL_F4,   // darkest
    CP_WALL_F5,   // darkest
    CP_WALL_F6,   // darkest
    CP_WALL_F7,   // darkest
    CP_WALL_F8,   // darkest
    // 3D view — side face shades (bright → dark, always darker than front)
    CP_WALL_S1,
    CP_WALL_S2,
    CP_WALL_S3,
    CP_WALL_S4,
    CP_WALL_S5,
    CP_WALL_S6,
    CP_WALL_S7,
    CP_WALL_S8,


    CP_FLOOR,
    CP_CEIL,

    // minimap
    CP_MAP_P,
    CP_MAP_EMPTY,
    CP_WALL_M,

    // UI widgets
    CP_UI_BDR,
    CP_UI_GOOD,
    CP_UI_WARN,
    CP_UI_BAD,
    CP_UI_LABEL,
    CP_UI_TEXT,

    // gun
    CP_GUN,
    CP_FLASH,
    HAND_CLR,
    HAND_CLR_S,
    GUN_BDR,
    MUZ_1,
    MUZ_2,
    MUZ_3,
    GUN_ACC,
    GUN_BODY,
    GUN_TRIM,
    GUN_DIRT,

    // title
    TITLE1,
    TITLE2,
    TITLE3,
    TITLE4,
    TITLEBG,

    // entity — red (light → dark)
    CP_ENTITY_R1,   // light red
    CP_ENTITY_R2,   // mid red
    CP_ENTITY_R3,   // dark red

    // entity — yellow (light → dark)
    CP_ENTITY_Y1,   // light yellow
    CP_ENTITY_Y2,   // mid yellow
    CP_ENTITY_Y3,   // dark yellow

    // entity — blue (light → dark)
    CP_ENTITY_B1,   // light blue
    CP_ENTITY_B2,   // mid blue
    CP_ENTITY_B3,   // dark blue

    // entity — shared
    CP_ENTITY_W,    // white eye

    // LEGACY BUT TITLE USES
    CP_WALL4,
    CP_HUD,

    CP_COUNT   // total — used to size the table
} ColorPair;
#endif