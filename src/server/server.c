#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "network.h"
#include "server_socket.h"
#include "client_manager.h"


void print_server_map(Client *clients, nfds_t nfds)
{
    char grid[MAP_H][MAP_W];

    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            grid[y][x] = map_solid(x, y) ? '#' : '.';

    for (nfds_t i = 1; i < nfds; i++) {
        Entity *e = &clients[i].entity;
        int ex = (int)e->x;
        int ey = (int)e->y;
        if (ex < 0 || ex >= MAP_W || ey < 0 || ey >= MAP_H) continue;

        double a = fmod(e->angle, 2 * M_PI);
        if (a < 0) a += 2 * M_PI;

        char dir;
        if      (a < 1*M_PI/4) dir = '>';
        else if (a < 3*M_PI/4) dir = 'V';
        else if (a < 5*M_PI/4) dir = '<';
        else if (a < 7*M_PI/4) dir = '^';
        else                   dir = '>';

        grid[ey][ex] = dir;
    }

    printf("\033[2J\033[H");
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++)
            putchar(grid[y][x]);
        putchar('\n');
    }
    printf("clients: %d\n", (int)nfds - 1);
}

int main(void)
{
    int server_fd = setup_server_socket();
    struct pollfd *pfds    = NULL;
    Client        *clients = NULL;
    nfds_t capacity = INITIAL_CLIENTS;
    nfds_t nfds     = 0;

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
                    // 1.updates capacity
                    // 2.adds new client to pfds and clients
                    // 3.sends itself to the new client (to get its own id)
                    // 4.sends existing clients to the new client
                    // 5.broadcasts itself to existing clients
                    add_client(new_socket, &pfds, &clients, &nfds, &capacity);
                    print_server_map(clients, nfds);
                } else {
                    if (handle_client_input(i, pfds, clients, &nfds))
                        i--;
                    
                    print_server_map(clients, nfds);
                }
            } else if (pfds[i].revents & (POLLHUP | POLLERR)) {
                remove_client(i, pfds, clients, &nfds);
                i--;
                
                    print_server_map(clients, nfds);
            }
        }
    }

    free(pfds);
    free(clients);
    close(server_fd);
    return 0;
}

