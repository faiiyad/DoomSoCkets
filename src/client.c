#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h> 
#include "network.h"
#include "client_socket.h"
#include "client.h"
#include "entity.h"
#include "defs.h"
#include "client_fn.h"

static struct pollfd *pfds    = NULL;
static int            sock_fd = -1;



int client_connect(const char *host, int port)
{
    init_client_state(host, port, &pfds, &sock_fd);
    return sock_fd;
}

void client_send_position(double x, double y, double angle, int dmg)
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
    int len;
    if(dmg == 0) {
        len = snprintf(buf, sizeof(buf), "%.2f %.2f %.2f\n", x, y, angle);
    } else {
        len = snprintf(buf, sizeof(buf), "%.2f %.2f %.2f %d\n", x, y, angle, dmg);
    }
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

void client_recv_updates(Player *player)
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
                    on_server_remove(id);
            } else if (strncmp(line, "KILL", 4) == 0) {
                int killer_id, victim_id;
                if (sscanf(line, "KILL %d %d", &killer_id, &victim_id) == 2)
                    on_server_kill(killer_id, victim_id, player);}
            else if (strncmp(line, "WIN", 3) == 0){
                int killer_id, victim_id;
                if (sscanf(line, "WIN %d %d", &killer_id, &victim_id) == 2)
                    on_server_win(killer_id, victim_id, player);
                
            }
            else if(strncmp(line, "RESPAWN", 7) == 0) {
                int respawn_id;
                double new_x, new_y;
                if (sscanf(line, "RESPAWN %d %lf %lf", &respawn_id, &new_x, &new_y) == 3)
                    on_server_respawn(respawn_id, new_x, new_y, player);
            }
            else if (sscanf(line, "%d %c %lf %lf %lf %d %d",
                        &u.id, &u.col, &u.x, &u.y, &u.angle, &u.health, &u.kills) == 7) {
                on_server_update(u, player);
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
void client_recv_initial(Player *p)
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
        // printf("initial data: %s\n", line);
        if (sscanf(line, "%d %c %lf %lf %lf %d %d",
                   &u.id, &u.col, &u.x, &u.y, &u.angle, &u.health, &u.kills) == 7) {
            if (first) {
                // own_id = u.id;  // first line is always yourself
                // p->id = u.id;
                // p->x = u.x;
                // p->y = u.y;
                // p->angle = u.angle;
                // p->health = u.health;
                // p->col = col_from_char(u.col);
                // p->kills = u.kills;
                on_server_initial(u, p);
                first = 0;
            } else {
                on_server_update(u, p);
            }
        }
        line = newline + 1;
    }
}

// int client_get_own_id(void)     { return own_id;     }
// int client_get_own_health(void) { return own_health; }
// int client_pop_hit(void)        { int v = hit_flag; hit_flag = 0; return v; }
// int client_get_kills(void)      { return kill_count; }