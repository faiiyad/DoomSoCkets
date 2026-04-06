#ifndef CLIENT_FN_H
#define CLIENT_FN_H


void on_server_update(ClientUpdate u);
void on_server_remove(int id);
void on_server_kill(int killer_id, int victim_id);
void on_server_win(double win_x, double win_y);
void on_server_respawn(int respawn_id, double new_x, double new_y);

#endif