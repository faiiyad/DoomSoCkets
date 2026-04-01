#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_manager.h"

static int next_entity_id = 1;

void update_capacity(struct pollfd **pfds, Client **clients,
                     nfds_t *capacity, nfds_t nfds)
{
    if (nfds < *capacity) return;

    nfds_t new_capacity = *capacity * 2;
    struct pollfd *new_pfds    = realloc(*pfds,    new_capacity * sizeof(struct pollfd));
    Client        *new_clients = realloc(*clients, new_capacity * sizeof(Client));
    if (new_pfds == NULL || new_clients == NULL) {
        perror("realloc");
        exit(1);
    }

    *pfds    = new_pfds;
    *clients = new_clients;

    for (nfds_t i = *capacity; i < new_capacity; i++) {
        (*pfds)[i].fd        = -1;
        (*clients)[i].fd     = -1;
        (*clients)[i].entity.id = -1;
    }

    *capacity = new_capacity;
}

void remove_client(nfds_t idx, struct pollfd *pfds, Client *clients,
                   nfds_t *nfds)
{
    close(pfds[idx].fd);
    pfds[idx]     = pfds[*nfds - 1];
    clients[idx]  = clients[*nfds - 1];
    (*nfds)--;
}

void add_client(int new_socket, struct pollfd **pfds, Client **clients,
                nfds_t *nfds, nfds_t *capacity)
{
    update_capacity(pfds, clients, capacity, *nfds);

    int client_idx = *nfds;
    (*pfds)[client_idx].fd       = new_socket;
    (*pfds)[client_idx].events   = POLLIN;

    (*clients)[client_idx].fd           = new_socket;
    (*clients)[client_idx].entity.id    = next_entity_id++;
    (*clients)[client_idx].entity.x     = 0.0;
    (*clients)[client_idx].entity.y     = 0.0;
    (*clients)[client_idx].entity.angle = 0.0;
    (*clients)[client_idx].entity.health = 100;

    (*nfds)++;

    printf("new client assigned entity id %d idx %d\n",
           (*clients)[client_idx].entity.id, client_idx);
    printf("new connection %d\n", new_socket);
}

int handle_client_input(nfds_t idx, struct pollfd *pfds, Client *clients,
                        nfds_t *nfds)
{
    char    buffer[1024];
    ssize_t s = read(pfds[idx].fd, buffer, sizeof(buffer) - 1);
    if (s <= 0) {
        if (s < 0) perror("read");
        remove_client(idx, pfds, clients, nfds);
        return 1;
    }

    buffer[s] = '\0';
    double x, y, angle;
    int hit_id;
    int count = sscanf(buffer, "%lf %lf %lf %d", &x, &y, &angle, &hit_id);
    if (count >= 3) {
        clients[idx].entity.x     = x;
        clients[idx].entity.y     = y;
        clients[idx].entity.angle = angle;

        if (count == 4)
            printf("entity id %d hit entity id %d\n",
                   clients[idx].entity.id, hit_id);

        printf("updated entity id %d => x=%.2f y=%.2f angle=%.2f\n",
               clients[idx].entity.id, x, y, angle);
    } else {
        printf("invalid entity update from fd %d: %s\n", pfds[idx].fd, buffer);
    }
    return 0;
}