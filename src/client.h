#ifndef CLIENT_H
#define CLIENT_H

#include "player.h"

int  client_connect(const char *host, int port);
int client_get_own_id(void);

void client_send_position(double x, double y, double angle, int id);
int client_is_connected(void);
void client_disconnect(void);
// Add these to client.h:
int  client_get_own_health(void);   // our health as server sees it
int  client_pop_hit(void);          // returns 1 once if we were just hit, then resets
int  client_get_kills(void);        // our kill count

typedef struct {
    int    id;
    char   col;
    double x, y, angle;
    int    health;
    int    kills;
} ClientUpdate;

void client_recv_updates(void (*on_update)(ClientUpdate), void (*on_remove)(int), void (*on_kill)(int, int));
void client_recv_initial(Player *player, void (*on_update)(ClientUpdate));

#endif