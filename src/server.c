#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "network.h"


int main() {
    nfds_t nfds, capacity = INITIAL_CLIENTS;
    struct pollfd *pfds;
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    pfds = calloc(capacity, sizeof(struct pollfd));
    if (pfds == NULL) {
        perror("calloc");
        exit(1);
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(NETWORK_PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        exit(1);
    }

    //SOMAXCONN is max # of connections queued but its like 4096 so its kinda huge
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        exit(1);
    }

    for (nfds_t i = 0; i < capacity; i++) {
        pfds[i].fd = -1;
    }
    pfds[0].fd = server_fd;
    pfds[0].events = POLLIN;
    nfds = 1;
    
    printf("starting serveerr\n");
    while(1) {
        int poll_count = poll(pfds, nfds, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (nfds_t i = 0; i < nfds; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == server_fd) {
                    int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
                    if (new_socket == -1) { 
                        perror("accept"); 
                        continue; 
                    }

                    if (nfds >= capacity) {
                        size_t new_capacity = capacity * 2;
                        struct pollfd *temp = realloc(pfds, new_capacity * sizeof(struct pollfd));
                        if (temp == NULL) { 
                            perror("realloc"); 
                            exit(1); 
                        }
                        pfds = temp;
                        for (nfds_t j = capacity; j < new_capacity; j++) pfds[j].fd = -1;
                        printf("pdfs capacity increased from %zu to %zu\n", capacity, new_capacity);
                        capacity = new_capacity;
                    }

                    pfds[nfds].fd = new_socket;
                    pfds[nfds].events = POLLIN;
                    nfds++;
                    printf("new guy\n");
                } else {
                    ssize_t s = read(pfds[i].fd, buffer, sizeof(buffer));
                    if (s <= 0) {
                        if (s < 0) perror("read");
                        close(pfds[i].fd);
                        pfds[i] = pfds[nfds - 1];
                        nfds--;
                        i--;
                    } else {
                        printf("receive %.*s\n", (int)s, buffer);
                    }
                }

            } else if (pfds[i].revents & (POLLHUP | POLLERR)) {
                close(pfds[i].fd);
                pfds[i] = pfds[nfds - 1];
                nfds--;
            } 
        }
    }
}