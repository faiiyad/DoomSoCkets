#ifndef UI_H
#define UI_H

#include "player.h"

void ui_draw_hud      (Player *p);
void ui_draw_controls (int rows);
void ui_draw_server   (int rows);
void ui_draw_eventlog (int rows, int cols);
void ui_log_event     (const char *msg);
void ui_draw_minimap  (Player *p, int rows, int cols);

#endif