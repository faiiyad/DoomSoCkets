#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include <poll.h>
#include "network.h"

void init_client_state(const char *host, int port, struct pollfd **pfds,
                       int *sock_fd);

#endif