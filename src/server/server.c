#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "network.h"
#include "server_socket.h"
#include "client_manager.h"

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
                    add_client(new_socket, &pfds, &clients, &nfds, &capacity);
                } else {
                    if (handle_client_input(i, pfds, clients, &nfds))
                        i--;
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