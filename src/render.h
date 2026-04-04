#ifndef RENDER_H
#define RENDER_H

#include "player.h"

#define N_SHADES 10
static const char *SHADES = " .:-+*=#%@";

void render(Player *p, int show_map);

void trigger_hit_indicator(void);
void draw_hit_indicator(int rows, int cols);
void draw_hit_indicator_loop(int rows, int cols);
void draw_face(int rows, int cols);
void trigger_face_glow(int krab);

#endif