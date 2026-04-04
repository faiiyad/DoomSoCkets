#ifndef GUN_H
#define GUN_H

#define GUN_COUNT 5
#define MAX_MUZZLES 4


typedef struct {
    int row;
    int col;
} MuzzlePos;

typedef struct {
    char *name;
    const char **art;
    const char **clr;
    int dmg;
    int cx;
    int r;
    int num_art_rows;
    MuzzlePos muzzles[MAX_MUZZLES];
    int num_muzzles;
} Gun;

extern int gun_frame;
extern int gun_timer;
extern float gun_offset;
extern float gun_offset2;
extern Gun guns[GUN_COUNT];

void draw_gun(int gun_idx, int rows, int cols);
void init_guns(void);

#endif