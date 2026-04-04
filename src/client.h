#ifndef CLIENT_H
#define CLIENT_H

#include "player.h"

int  client_connect(const char *host, int port);
int client_get_own_id(void);

void client_send_position(double x, double y, double angle, int id);
int client_is_connected(void);
void client_disconnect(void);


typedef struct {
    int    id;
    char   col;
    double x, y, angle;
    int    health;
} ClientUpdate;

void client_recv_updates(void (*on_update)(ClientUpdate), void (*on_remove)(int));
void client_recv_initial(Player *player, void (*on_update)(ClientUpdate));

#endif