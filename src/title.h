#ifndef RAYCASTER_TITLE_H
#define RAYCASTER_TITLE_H

#include "player.h"
#include "entity.h"

void show_title_screen(void);
void show_death_screen(void);
void show_end_screen(Player *player, Entity *entities, int num_entities);
void name_printer(char *name, int index, char c);

#endif 