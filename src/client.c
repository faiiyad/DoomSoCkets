#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h> 
#include "network.h"
#include "client_socket.h"
#include "client.h"

static struct pollfd *pfds    = NULL;
static int            sock_fd = -1;

int client_connect(const char *host, int port)
{
    init_client_state(host, port, &pfds, &sock_fd);
    return sock_fd;
}

void client_send_position(double x, double y, double angle, int id)
{
    if (sock_fd == -1) {
        fprintf(stderr, "client_send_position: not connected\n");
        return;
    }

    struct pollfd pfd = { .fd = sock_fd, .events = POLLOUT };
    int ready = poll(&pfd, 1, 1000);

    if (ready == -1) { perror("poll");                                    return; }
    if (ready ==  0) { fprintf(stderr, "send timed out\n");               return; }
    if (pfd.revents & (POLLHUP | POLLERR)) { fprintf(stderr, "socket error\n"); return; }

    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%.2f %.2f %.2f %d\n", x, y, angle, id);
    if (write(sock_fd, buf, len) == -1)
        perror("write");
}

void client_disconnect(void)
{
    if (sock_fd != -1) {
        close(sock_fd);
        sock_fd = -1;
    }
    free(pfds);
    pfds = NULL;
}


void client_recv_updates(void (*on_update)(ClientUpdate))
{
    if (sock_fd == -1) return;

    // Check if data is available without blocking
    struct pollfd pfd = { .fd = sock_fd, .events = POLLIN };
    while (poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN)) {
        static char buf[4096];
        static int  buf_len = 0;

        ssize_t n = read(sock_fd, buf + buf_len, sizeof(buf) - buf_len - 1);
        if (n <= 0) {
            if (n < 0) perror("read");
            client_disconnect();
            return;
        }
        buf_len += n;
        buf[buf_len] = '\0';

        // Process all complete lines
        char *line = buf;
        char *newline;
        while ((newline = strchr(line, '\n')) != NULL) {
            *newline = '\0';

            ClientUpdate u;
            if (sscanf(line, "%d %lf %lf %lf %d",
                       &u.id, &u.x, &u.y, &u.angle, &u.health) == 5) {
                on_update(u);
            }
            line = newline + 1;
        }

        // Move leftover incomplete line to front of buffer
        buf_len = (buf + buf_len) - line;
        memmove(buf, line, buf_len);

        pfd.revents = 0;
    }
}