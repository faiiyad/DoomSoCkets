#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "network.h"
#include "server_socket.h"
#include "client_manager.h"
#include "server_ui.h"

int main(void)
{
    int server_fd = setup_server_socket();
    struct pollfd *pfds    = NULL;
    Client        *clients = NULL;
    nfds_t capacity = INITIAL_CLIENTS;
    nfds_t nfds     = 0;

    init_server_state(server_fd, &pfds, &clients, &capacity, &nfds);
    init_server_ui();
    draw_server_ui(clients, nfds);

    int running = 1;
    while (running) {
        int poll_count = poll(pfds, nfds, 100);
        if (poll_count == -1) {
            server_log("poll error: %s", strerror(errno));
            break;
        }

        if (poll_count > 0) {
            for (nfds_t i = 0; i < nfds; i++) {
                if (pfds[i].revents & POLLIN) {
                    if (pfds[i].fd == server_fd) {
                        struct sockaddr_in address;
                        socklen_t addrlen = sizeof(address);
                        int new_socket = accept(server_fd,
                                                (struct sockaddr *)&address,
                                                &addrlen);
                        if (new_socket == -1) {
                            server_log("accept error: %s", strerror(errno));
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

        draw_server_ui(clients, nfds);
        int ch = server_ui_get_input();
        if (ch == 'q' || ch == 'Q') {
            running = 0;
        }
    }

    shutdown_server_ui();
    free(pfds);
    free(clients);
    close(server_fd);
    return 0;
}

