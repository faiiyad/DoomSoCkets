#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <poll.h>
#include "entity.h"
#include "map.h"
#include "ray.h"

typedef struct {
    int    fd;
    Entity entity;
    int    kills;   // server-side kill count for this client
} Client;

void remove_client(nfds_t idx, struct pollfd *pfds, Client *clients,
                   nfds_t *nfds);
void add_client(int new_socket, struct pollfd **pfds, Client **clients,
                nfds_t *nfds, nfds_t *capacity);
int  handle_client_input(nfds_t idx, struct pollfd *pfds, Client *clients,
                         nfds_t *nfds);

#endif