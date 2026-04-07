#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client_socket.h"
#include "ui.h"

int setup_client_socket(const char *host, int port)
{
    int sock_fd;
    struct sockaddr_in address;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port   = htons(port);
    if (inet_pton(AF_INET, host, &address.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        return -1;
    }

    if (connect(sock_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        // perror("connect-client");
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

void init_client_state(const char *host, int port, struct pollfd **pfds,
                       int *sock_fd)
{
    *pfds = calloc(1, sizeof(struct pollfd));
    if (*pfds == NULL) {
        perror("calloc");
        exit(1);
    }

    *sock_fd = setup_client_socket(host, port);
    if (*sock_fd == -1) {
        ui_log_event("Server Offline");
        return;
    }

    (*pfds)[0].fd     = *sock_fd;
    (*pfds)[0].events = POLLIN;
    
    char buf[64];
    snprintf(buf, sizeof(buf), "Connected to %s:%d", host, port);
    ui_log_event(buf);
}