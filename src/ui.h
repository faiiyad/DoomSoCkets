#ifndef UI_H
#define UI_H

#include "player.h"

// stats bar — top left, horizontal
void ui_draw_hud      (Player *p);

// key bindings — bottom left, vertical
void ui_draw_controls (int rows);

// server/entity panel — left, between HUD and keys
void ui_draw_server   (int rows);

// toggle server connection placeholder — bind to 'C' in main
void ui_toggle_connect(void);

// event log — bottom right, below minimap
void ui_draw_eventlog (int rows, int cols);

// push a message into the event log — call from anywhere
void ui_log_event     (const char *msg);

// minimap — top right
void ui_draw_minimap  (Player *p, int rows, int cols);

#endif