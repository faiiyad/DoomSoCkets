#ifndef UI_H
#define UI_H

#include "player.h"

void ui_draw_minimap (Player *p, int rows, int cols);
void ui_draw_hud     (Player *p);
void ui_draw_controls(int rows);

#endif