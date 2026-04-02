#ifndef CLIENT_H
#define CLIENT_H

int  client_connect(const char *host, int port);
void client_send_position(double x, double y, double angle, int id);
void client_disconnect(void);

typedef struct {
    int    id;
    double x, y, angle;
    int    health;
} ClientUpdate;

void client_recv_updates(void (*on_update)(ClientUpdate), void (*on_remove)(int));

#endif