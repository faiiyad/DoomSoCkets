#ifndef RAYCASTER_TITLE_H
#define RAYCASTER_TITLE_H

#include "player.h"
#include "entity.h"

/* Blocks until the player presses Enter */
void show_title_screen(void);
void show_death_screen(Player *player);
void show_end_screen(Player *player, Entity *entities, int num_entities);
void name_printer(char *name, int index, char c);

#endif /* RAYCASTER_TITLE_H */