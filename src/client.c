#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h> 
#include "network.h"
#include "client_socket.h"
#include "client.h"
#include "player.h"
#include "entity.h"
#include "defs.h"

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

int client_is_connected(void)
{
    return sock_fd != -1;
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


void client_recv_updates(Player *p, void (*on_update)(ClientUpdate), void (*on_remove)(int))
{
    if (sock_fd == -1) return;

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

        char *line = buf;
        char *newline;
        while ((newline = strchr(line, '\n')) != NULL) {
            *newline = '\0';

            int id;
            ClientUpdate u;
            if (strncmp(line, "REMOVE", 6) == 0) {
                if (sscanf(line, "REMOVE %d", &id) == 1)
                    on_remove(id);
            } else if (strncmp(line, "KILL", 4) == 0) {
                // printf("%d", p->kill_count);
                p->kill_count++;
            } else if (sscanf(line, "%d %c %lf %lf %lf %d",
                        &u.id, &u.col, &u.x, &u.y, &u.angle, &u.health) == 6) {
                if (u.id == p->id) {
                    if (u.health < p->health){
                        p->health = u.health;
                    }
                    
                } else {
                    on_update(u);
                }
            }
            line = newline + 1;
        }

        buf_len = (buf + buf_len) - line;
        memmove(buf, line, buf_len);
        pfd.revents = 0;
    }
}


// static int own_id = -1;

// in client_recv_initial, the FIRST line is always your own entity
void client_recv_initial(Player *p, void (*on_update)(ClientUpdate))
{
    if (sock_fd == -1) return;

    struct pollfd pfd = { .fd = sock_fd, .events = POLLIN };
    if (poll(&pfd, 1, 500) <= 0) return;

    char buf[4096];
    ssize_t n = read(sock_fd, buf, sizeof(buf) - 1);
    if (n <= 0) return;
    buf[n] = '\0';

    char *line = buf;
    char *newline;
    int first = 1;
    while ((newline = strchr(line, '\n')) != NULL) {
        *newline = '\0';
        ClientUpdate u;
        if (sscanf(line, "%d %c %lf %lf %lf %d",
                   &u.id, &u.col, &u.x, &u.y, &u.angle, &u.health) == 6) {
            if (first) {
                p->id = u.id;  // first line is always yourself
                p->x = u.x;
                p->y = u.y;
                p->angle = u.angle;
                p->health = u.health;
                p->col = col_from_char(u.col);
                first = 0;
            } else {
                on_update(u);
            }
        }
        line = newline + 1;
    }
}