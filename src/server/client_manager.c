#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "client_manager.h"

void server_log(const char *fmt, ...);

static int next_entity_id = 1;

// Batch entities to 1 client
static void send_entities_batch(int fd, Client *clients, nfds_t nfds, int exclude_fd)
{
    char buf[4096];
    int  offset = 0;

    for (nfds_t i = 1; i < nfds; i++) {
        if (clients[i].fd == exclude_fd) continue;
        Entity *e = &clients[i].entity;
        offset += snprintf(buf + offset, sizeof(buf) - offset,
                           "%d %.2f %.2f %.2f %d\n",
                           e->id, e->x, e->y, e->angle, e->health);
    }

    if (offset > 0)
        if (write(fd, buf, offset) == -1) {
            server_log("write failed: %s", strerror(errno));
        }
}

// One entity to 1 client
static void send_one_entity(int fd, const Entity *e)
{
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%d %.2f %.2f %.2f %d\n",
                       e->id, e->x, e->y, e->angle, e->health);
    if (write(fd, buf, len) == -1) {
        server_log("write failed: %s", strerror(errno));
    }
}

// One entity to all clients
static void broadcast_entity(const Entity *e, Client *clients, 
                            nfds_t nfds, int exclude_fd)
{
    for (nfds_t i = 1; i < nfds; i++) {
        if (clients[i].fd != exclude_fd)
            send_one_entity(clients[i].fd, e);
    }
}

static void update_capacity(struct pollfd **pfds, Client **clients,
                     nfds_t *capacity, nfds_t nfds)
{
    if (nfds < *capacity) return;

    nfds_t new_capacity = *capacity * 2;
    struct pollfd *new_pfds    = realloc(*pfds,    new_capacity * sizeof(struct pollfd));
    Client        *new_clients = realloc(*clients, new_capacity * sizeof(Client));
    if (new_pfds == NULL || new_clients == NULL) {
        server_log("realloc failed: %s", strerror(errno));
        exit(1);
    }

    *pfds    = new_pfds;
    *clients = new_clients;

    for (nfds_t i = *capacity; i < new_capacity; i++) {
        (*pfds)[i].fd        = -1;
        (*clients)[i].fd     = -1;
        (*clients)[i].entity.id = -1;
    }
    
    server_log("increased capacity from %zu to %zu", *capacity, new_capacity);
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
    (*pfds)[client_idx].revents = 0; 

    (*clients)[client_idx].fd           = new_socket;
    (*clients)[client_idx].entity.id    = next_entity_id++;
    (*clients)[client_idx].entity.x     = 0.0;
    (*clients)[client_idx].entity.y     = 0.0;
    (*clients)[client_idx].entity.angle = 0.0;
    (*clients)[client_idx].entity.health = 100;

    (*nfds)++;

    // Send itself to the new client
    send_one_entity(new_socket, &(*clients)[client_idx].entity);

    // Send existing entities to the new client
    send_entities_batch(new_socket, *clients, *nfds - 1, new_socket);

    // Broadcast the new client's entity to existing clients
    broadcast_entity(&(*clients)[client_idx].entity, *clients, *nfds, new_socket);

    server_log("new client assigned entity id %d idx %d",
               (*clients)[client_idx].entity.id, client_idx);
    server_log("new connection %d", new_socket);
}

int handle_client_input(nfds_t idx, struct pollfd *pfds, Client *clients,
                        nfds_t *nfds)
{
    char    buffer[1024];
    ssize_t s = read(pfds[idx].fd, buffer, sizeof(buffer) - 1);
    if (s <= 0) {
        if (s < 0) server_log("read error on fd %d: %s", pfds[idx].fd, strerror(errno));
        remove_client(idx, pfds, clients, nfds);
        return 1;
    }

    buffer[s] = '\0';
    double x, y, angle;
    int damage;
    int count = sscanf(buffer, "%lf %lf %lf %d", &x, &y, &angle, &damage);
    if (count >= 3) {
        clients[idx].entity.x     = x;
        clients[idx].entity.y     = y;
        clients[idx].entity.angle = angle;

        if (count == 4) {
            for (nfds_t i = 1; i < *nfds; i++) {
                if (i == idx) continue;
                int hit;
                double dist = cast_ray_to_entity(x, y, angle, &clients[i].entity, 0.5, &hit);
                if (hit) {
                    clients[i].entity.health -= damage;
                    server_log("entity id %d hit entity id %d (health now %d)",
                               clients[idx].entity.id, clients[i].entity.id,
                               clients[i].entity.health);
                    broadcast_entity(&clients[i].entity, clients,
                                    *nfds, -1);
                }
            }
        }

        server_log("updated entity id %d => x=%.2f y=%.2f angle=%.2f",
                   clients[idx].entity.id, x, y, angle);
                   
        broadcast_entity(&clients[idx].entity, clients, *nfds,
                     clients[idx].fd);
    } else {
        server_log("invalid entity update from fd %d: %s", pfds[idx].fd, buffer);
    }
    return 0;
}
