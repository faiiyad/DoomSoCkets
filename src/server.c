#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "network.h"
#include "map.h"
#include "ray.h"
#include "entity.h"

typedef struct {
    int fd;
    Entity entity;
} Client;

static int next_entity_id = 1;

static int setup_server_socket(void)
{
    int server_fd;
    int opt = 1;
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        exit(1);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(NETWORK_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    return server_fd;
}

static void init_server_state(int server_fd, struct pollfd **pfds, 
                            Client **clients, nfds_t *capacity, nfds_t *nfds)
{
    *pfds = calloc(*capacity, sizeof(struct pollfd));
    *clients = calloc(*capacity, sizeof(Client));
    if (*pfds == NULL || *clients == NULL) {
        perror("calloc");
        exit(1);
    }

    for (nfds_t i = 0; i < *capacity; i++) {
        (*pfds)[i].fd = -1;
        (*clients)[i].fd = -1;
        (*clients)[i].entity.id = -1;
    }

    (*pfds)[0].fd = server_fd;
    (*pfds)[0].events = POLLIN;
    *nfds = 1;
}

static void update_capacity(struct pollfd **pfds, Client **clients, 
                        nfds_t *capacity, nfds_t nfds)
{
    if (nfds < *capacity) return;

    size_t new_capacity = *capacity * 2;
    struct pollfd *new_pfds = realloc(*pfds, new_capacity * sizeof(struct pollfd));
    Client *new_clients = realloc(*clients, new_capacity * sizeof(Client));
    if (new_pfds == NULL || new_clients == NULL) {
        perror("realloc");
        exit(1);
    }

    *pfds = new_pfds;
    *clients = new_clients;

    for (nfds_t i = *capacity; i < new_capacity; i++) {
        (*pfds)[i].fd = -1;
        (*clients)[i].fd = -1;
        (*clients)[i].entity.id = -1;
    }

    *capacity = new_capacity;
}

static void remove_client(nfds_t idx, struct pollfd *pfds, Client *clients, 
                        nfds_t *nfds)
{
    close(pfds[idx].fd);
    pfds[idx] = pfds[*nfds - 1];
    clients[idx] = clients[*nfds - 1];
    (*nfds)--;
}

static void add_client(int new_socket, struct pollfd **pfds, Client **clients,
                       nfds_t *nfds, nfds_t *capacity)
{
    update_capacity(pfds, clients, capacity, *nfds);

    int client_idx = *nfds;
    (*pfds)[client_idx].fd = new_socket;
    (*pfds)[client_idx].events = POLLIN;

    (*clients)[client_idx].fd = new_socket;
    (*clients)[client_idx].entity.id = next_entity_id++;
    (*clients)[client_idx].entity.x = 0.0;
    (*clients)[client_idx].entity.y = 0.0;
    (*clients)[client_idx].entity.angle = 0.0;
    (*clients)[client_idx].entity.health = 100;

    (*nfds)++;

    printf("new client assigned entity id %d idx %d\n",
           (*clients)[client_idx].entity.id, client_idx);
    printf("new connection %d\n", new_socket);
}

static int handle_client_input(nfds_t idx, struct pollfd *pfds, Client *clients,
    nfds_t *nfds)
{
    char buffer[1024];
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
        clients[idx].entity.x = x;
        clients[idx].entity.y = y;
        clients[idx].entity.angle = angle;

        if (count == 4) {
            printf("entity id %d hit entity id %d\n",
                   clients[idx].entity.id, hit_id);
        }

        printf("updated entity id %d => x=%.2f y=%.2f angle=%.2f\n",
               clients[idx].entity.id, x, y, angle);
    } else {
        printf("invalid entity update from fd %d: %s\n", pfds[idx].fd, buffer);
    }
    return 0;
}

static double server_cast_ray(double px, double py, double angle,
    int *side, int *wtype)
{
    return cast_ray(px, py, angle, side, wtype);
}

int main(void)
{
    int server_fd = setup_server_socket();
    struct pollfd *pfds = NULL;
    Client *clients = NULL;
    nfds_t capacity = INITIAL_CLIENTS;
    nfds_t nfds = 0;

    init_server_state(server_fd, &pfds, &clients, &capacity, &nfds);

    printf("starting server\n");
    while (1) {
        int poll_count = poll(pfds, nfds, -1);
        if (poll_count == -1) {
            perror("poll");
            break;
        }

        for (nfds_t i = 0; i < nfds; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == server_fd) {
                    struct sockaddr_in address;
                    socklen_t addrlen = sizeof(address);
                    int new_socket = accept(server_fd,
                                            (struct sockaddr *)&address,
                                            &addrlen);
                    if (new_socket == -1) {
                        perror("accept");
                        continue;
                    }
                    add_client(new_socket, &pfds, &clients, &nfds, &capacity);
                } else {
                    if (handle_client_input(i, pfds, clients, &nfds)) {
                        i--;
                    }
                }
            } else if (pfds[i].revents & (POLLHUP | POLLERR)) {
                remove_client(i, pfds, clients, &nfds);
                i--;
            }
        }
    }

    free(pfds);
    free(clients);
    close(server_fd);
    return 0;
}
