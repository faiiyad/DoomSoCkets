#ifndef RENDER_H
#define RENDER_H

#include "player.h"

#define N_SHADES 10
static const char *SHADES = " .:-+*=#%@";

void render(Player *p, int show_map);

#endif