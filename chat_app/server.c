#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "port.h"

int main() {
    // create a socket
    int listen_soc = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_soc == -1) {
        perror("socket");
        exit(1);
    }

    // setting up address
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memset(&server.sin_zero, 0, 8);
    server.sin_addr.s_addr = INADDR_ANY;

    // select config
    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(listen_soc, &master_set);
    int max_fd = listen_soc;

    // bind
    if (bind(listen_soc, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        close(listen_soc);
        exit(1);
    }

    // listen
    if (listen(listen_soc, 5) < 0) {
        perror("listen failed");
        exit(1);
    }

    while (1) {
        read_set = master_set;
        int activity = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select");
            exit(1);
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_set)) {

                // CASE 1: New connection
                if (fd == listen_soc) {
                    int client = accept(listen_soc, NULL, NULL);
                    if (client < 0) {
                        perror("accept");
                        continue;
                    }
                    FD_SET(client, &master_set);
                    if (client > max_fd)
                        max_fd = client;
                    printf("New client: %d\n", client);

                // CASE 2: Existing client sent data
                } else {
                    char buf[1024];
                    int n = read(fd, buf, sizeof(buf) - 1);

                    if (n <= 0) {
                        // FIX: disconnect handling stays separate, no broadcast
                        printf("Client %d disconnected\n", fd);
                        close(fd);
                        FD_CLR(fd, &master_set);

                        // FIX: shrink max_fd if needed
                        if (fd == max_fd) {
                            while (max_fd > listen_soc && !FD_ISSET(max_fd, &master_set))
                                max_fd--;
                        }
                    } else {
                        // printf("we have reached the else branch for buf");
                        // FIX: null-terminate before any printf
                        // buf[n] = '\0';
                        buf[strcspn(buf, "\r\n")] = '\0';
                        // printf("original buf msg for server: %s\n", buf);
                        printf("Client %d: %s\n", fd, buf);

                        // FIX: broadcast is inside else, and uses fd (sender) not i (recipient)
                        for (int i = 0; i <= max_fd; i++) {
                            if (FD_ISSET(i, &master_set)) {
                                if (i != listen_soc && i != fd) {
                                    char out[1024];
                                    int len = snprintf(out, sizeof(out),
                                        "Client %d: %s\n",
                                        i, buf);  // FIX: fd not i
                                    write(i, out, len);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
