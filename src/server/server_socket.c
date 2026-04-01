#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network.h"
#include "server_socket.h"

int setup_server_socket(void)
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

void init_server_state(int server_fd, struct pollfd **pfds,
                       Client **clients, nfds_t *capacity, nfds_t *nfds)
{
    *pfds    = calloc(*capacity, sizeof(struct pollfd));
    *clients = calloc(*capacity, sizeof(Client));
    if (*pfds == NULL || *clients == NULL) {
        perror("calloc");
        exit(1);
    }

    for (nfds_t i = 0; i < *capacity; i++) {
        (*pfds)[i].fd        = -1;
        (*clients)[i].fd     = -1;
        (*clients)[i].entity.id = -1;
    }

    (*pfds)[0].fd     = server_fd;
    (*pfds)[0].events = POLLIN;
    *nfds = 1;
}