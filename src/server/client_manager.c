#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "client_manager.h"
#include "network.h"


void server_log(const char *fmt, ...);

static int next_entity_id = 1;
static const char ENTITY_COLOURS[] = {'Y', 'B', 'R'};
static int next_colour_idx = 0;
static int spawn_index = 0;

double spawn[3][2] = {{11.2, 2.8}, {1.7, 2.7}, {13, 1.6}};



Spawn random_spawn(){
    Spawn r_spawn;
    r_spawn.x = spawn[spawn_index][0];
    r_spawn.y = spawn[spawn_index][1];
    spawn_index++;
    if (spawn_index >=3){
        spawn_index = 0;
    }
    return r_spawn;
}

// Batch entities to 1 client
static void send_entities_batch(int fd, Client *clients, nfds_t nfds, int first_fd)
{
    char buf[4096];
    int  offset = 0;

    for (nfds_t i = 1; i < nfds; i++) {
        if (clients[i].fd != first_fd) continue;
        server_log("sending initial entity %d to new client fd %d",
                   clients[i].entity.id, clients[i].fd);
        Entity *e = &clients[i].entity;
        offset += snprintf(buf + offset, sizeof(buf) - offset,
                           ENTITY_FMT, ENTITY_FMT_ARGS(e));
    }
    for (nfds_t i = 1; i < nfds; i++) {
        if (clients[i].fd == first_fd) continue;
        Entity *e = &clients[i].entity;
        offset += snprintf(buf + offset, sizeof(buf) - offset,
                           ENTITY_FMT, ENTITY_FMT_ARGS(e));
    }
    server_log("%s", buf);
    if (offset > 0)
        if (write(fd, buf, offset) == -1) {
            server_log("write failed: %s", strerror(errno));
        }
}

// One entity to 1 client
static void send_one_entity(int fd, const Entity *e)
{
    char buf[64];
    int len = snprintf(buf, sizeof(buf), ENTITY_FMT, ENTITY_FMT_ARGS(e));
    if (write(fd, buf, len) == -1) {
        server_log("write failed: %s", strerror(errno));
    }
}

// One entity to all clients
static void broadcast_entity(const Entity *e, Client *clients, 
                            nfds_t nfds, int exclude_fd)
{
    server_log("broadcasting entity id %d health=%d",
               e->id, e->health);
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
// void remove_client(nfds_t idx, struct pollfd *pfds, Client *clients,
//                    nfds_t *nfds)
// {
//     close(pfds[idx].fd);
//     pfds[idx]     = pfds[*nfds - 1];
//     clients[idx]  = clients[*nfds - 1];
//     (*nfds)--;
// }

void add_client(int new_socket, struct pollfd **pfds, Client **clients,
                nfds_t *nfds, nfds_t *capacity)
{
    update_capacity(pfds, clients, capacity, *nfds);

    Spawn r_spawn = random_spawn();

    int client_idx = *nfds;
    (*pfds)[client_idx].fd       = new_socket;
    (*pfds)[client_idx].events   = POLLIN;
    (*pfds)[client_idx].revents = 0;

    (*clients)[client_idx].fd           = new_socket;
    (*clients)[client_idx].entity.id    = next_entity_id++;
    (*clients)[client_idx].entity.x     = r_spawn.x;
    (*clients)[client_idx].entity.y     = r_spawn.y;
    (*clients)[client_idx].entity.angle = 0.0;
    (*clients)[client_idx].entity.health = 100;
    (*clients)[client_idx].entity.col    = ENTITY_COLOURS[next_colour_idx];
    (*clients)[client_idx].entity.kills         = 0;  
    next_colour_idx = (next_colour_idx + 1) % 3;

    (*nfds)++;

    // Send existing entities to the new client with the new client first in the batch
    send_entities_batch(new_socket, *clients, *nfds, new_socket);

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
    int count = sscanf(buffer, ENTITY_SCAN_FMT, &x, &y, &angle, &damage);
    if (count >= 3) {
        clients[idx].entity.x     = x;
        clients[idx].entity.y     = y;
        clients[idx].entity.angle = angle;

        if (x < -50 && y < -50) {
            clients[idx].entity.x = -100;
            clients[idx].entity.y = -100;
            broadcast_entity(&clients[idx].entity, clients, *nfds, clients[idx].fd);

            Spawn r_spawn = random_spawn();
            clients[idx].entity.health = 100;
            server_log("set respawn at %.2f, %.2f", r_spawn.x, r_spawn.y);
            char respawn_msg[32];
            int respawn_len = snprintf(respawn_msg, sizeof(respawn_msg), "RESPAWN %d %.2f %.2f\n",
                    clients[idx].entity.id, r_spawn.x, r_spawn.y);
            for (nfds_t j = 1; j < *nfds; j++) {
                            if (write(clients[j].fd, respawn_msg, respawn_len) == -1)
                                server_log("write failed: %s", strerror(errno));
                        }
            server_log("entity id %d respawned", clients[idx].entity.id);

            return 0;
        }
        
        if (count == 4) {
        server_log("CLI: id %d => x=%.2f y=%.2f angle=%.2f damage=%d",
                   clients[idx].entity.id, x, y, angle, damage);
            for (nfds_t i = 1; i < *nfds; i++) {
                if (clients[i].entity.id == clients[idx].entity.id) continue;
                if (clients[i].entity.health <= 0) continue;
                if (clients[i].entity.x < -90) continue; 
                int hit;
                cast_ray_to_entity(x, y, angle, &clients[i].entity, 0.5, &hit);
                if (hit) {
                    clients[i].entity.health -= damage;
                    server_log("SRV: id %d hit id %d (health now %d)",
                               clients[idx].entity.id, clients[i].entity.id,
                               clients[i].entity.health);
                    broadcast_entity(&clients[i].entity, clients, *nfds, -1);

                    if (clients[i].entity.health <= 0) {
                        char kill_msg[32];
                        int kill_len = snprintf(kill_msg, sizeof(kill_msg), "KILL %d %d\n",
                                                clients[idx].entity.id, clients[i].entity.id);
                        for (nfds_t j = 1; j < *nfds; j++) {
                            if (write(clients[j].fd, kill_msg, kill_len) == -1)
                                server_log("write failed: %s", strerror(errno));
                        }
                        clients[idx].entity.kills += 1;
                        server_log("SRV: id %d killed id %d (total kills: %d)",
                                clients[idx].entity.id, clients[i].entity.id,
                                clients[idx].entity.kills);
                        if (clients[idx].entity.kills >= 10) {

                            char win_msg[32];
                            int win_len = snprintf(win_msg, sizeof(win_msg), "WIN %d %d\n",
                                                -100, -100);
                            for (nfds_t j = 1; j < *nfds; j++) {
                                clients[j].entity.x = -100;
                                clients[j].entity.y = -100;
                                broadcast_entity(&clients[j].entity, clients, *nfds, clients[j].fd);
                                if (write(clients[j].fd, win_msg, win_len) == -1)
                                    server_log("write failed: %s", strerror(errno));                                
                            }
                            server_log("SRV: id %d wins!", clients[idx].entity.id);
                            for (nfds_t j = 1; j < *nfds; j++) {
                                clients[j].entity.kills = 0;
                                Spawn r_spawn = random_spawn();
                                clients[j].entity.x = r_spawn.x;
                                clients[j].entity.y = r_spawn.y;
                                clients[j].entity.health = 100;

                                char respawn_msg[32];
                                int respawn_len = snprintf(respawn_msg, sizeof(respawn_msg), "RESPAWN %d %.2f %.2f\n",
                                                        clients[j].entity.id, clients[j].entity.x, clients[j].entity.y);
                                for (nfds_t k = 1; k < *nfds; k++) {
                                    if (write(clients[k].fd, respawn_msg, respawn_len) == -1)
                                        server_log("write failed: %s", strerror(errno));
                                }
                            }
                        }
                    }
                }
            }
        } else {
        server_log("CLI: id %d => x=%.2f y=%.2f angle=%.2f",
                   clients[idx].entity.id, x, y, angle);
            broadcast_entity(&clients[idx].entity, clients, *nfds, clients[idx].fd);
        }

        // server_log("updated entity id %d => x=%.2f y=%.2f angle=%.2f",
                //    clients[idx].entity.id, x, y, angle);
                   
    } else {
        server_log("invalid entity update from fd %d: %s", pfds[idx].fd, buffer);
    }
    return 0;
}

void remove_client(nfds_t idx, struct pollfd *pfds, Client *clients,
                   nfds_t *nfds)
{
    // broadcast removal to all other clients
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "REMOVE %d\n",
                       clients[idx].entity.id);
    for (nfds_t i = 1; i < *nfds; i++) {
        if (i == idx) continue;
        if (write(clients[i].fd, buf, len) == -1)
            perror("write");
    }

    close(pfds[idx].fd);
    pfds[idx]    = pfds[*nfds - 1];
    clients[idx] = clients[*nfds - 1];
    (*nfds)--;
}