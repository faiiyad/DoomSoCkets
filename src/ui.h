#ifndef UI_H
#define UI_H

#include "player.h"

// stats bar at top left
void ui_draw_hud      (Player *p);

// key bindings at bottom left
void ui_draw_controls (int rows);

// server/entity panel at left, between HUD and keys
void ui_draw_server   (int rows);

// event log at bottom right, below the minimap
void ui_draw_eventlog (int rows, int cols);

// event log
void ui_log_event     (const char *msg);

// minimap at top right
void ui_draw_minimap  (Player *p, int rows, int cols);

#endif