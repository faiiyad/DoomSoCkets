#ifndef CLIENT_H
#define CLIENT_H

#include "player.h"


int  client_connect(const char *host, int port);

void client_send_position(double x, double y, double angle, int id);
int client_is_connected(void);
void client_disconnect(void);

typedef struct {
    int    id;
    char   col;
    double x, y, angle;
    int    health;
    int    kills;
} ClientUpdate;

void client_recv_updates(Player *player);
void client_recv_initial(Player *player);

#endif