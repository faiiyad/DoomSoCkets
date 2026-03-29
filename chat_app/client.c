#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "port.h"



int main(){

    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc == -1){
        perror("socket");
        exit(1);
    }



    struct sockaddr_in server;
    server.sin_family = AF_INET;
    memset(&server.sin_zero, 0, 8);
    server.sin_port = htons(PORT);

    struct addrinfo *result;

    getaddrinfo("localhost", NULL, NULL, &result);
    server.sin_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr;
    freeaddrinfo(result);



    int return_code = connect(soc, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
    printf("Connected %d\n", return_code);
    if (return_code == -1){
        perror("connect");
        exit(1);
    }

    fd_set read_set;

    while (1) {
        FD_ZERO(&read_set);

        FD_SET(STDIN_FILENO, &read_set); // keyboard
        FD_SET(soc, &read_set);          // server

        int max_fd = (soc > STDIN_FILENO ? soc : STDIN_FILENO);

        int activity = select(max_fd + 1, &read_set, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
            exit(1);
        }

        // 🔥 1. User typed something
        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            char input[1024];

            int n = read(STDIN_FILENO, input, sizeof(input));

            if (n <= 0) {
                printf("stdin closed\n");
                break;
            }

            write(soc, input, n);
        }

        // 🔥 2. Server sent something
        if (FD_ISSET(soc, &read_set)) {
            char buf[1024];

            int n = read(soc, buf, sizeof(buf) - 1);

            if (n <= 0) {
                printf("Server disconnected\n");
                break;
            }

            buf[n] = '\0';
            printf("%s", buf);
        }
    }

    return 0;


}
