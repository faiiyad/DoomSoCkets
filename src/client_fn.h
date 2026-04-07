#ifndef CLIENT_FN_H
#define CLIENT_FN_H

#include "player.h"
#include "entity.h"
#include "gun.h"
#include "client.h"

void death(Player *player);

void on_server_update(ClientUpdate u, Player *player);
void on_server_remove(int id);
void on_server_kill(int killer_id, int victim_id, Player *player);
void on_server_win(double win_x, double win_y, Player *player);
void on_server_respawn(int respawn_id, double new_x, double new_y, Player *player);

#endif