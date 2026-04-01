#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include <poll.h>
#include "client_manager.h"

int  setup_server_socket(void);
void init_server_state(int server_fd, struct pollfd **pfds,
                       Client **clients, nfds_t *capacity, nfds_t *nfds);

#endif