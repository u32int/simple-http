#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "response.h"
#include "args_parser.h"
#include "config.h"

char *ROOT_DIR = NULL;

int setup_local_socket()
{
    struct addrinfo hints, *servinfo;
    int servfd, yes=1;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;     /* local socket for accepting connections */
    hints.ai_family = AF_INET;       /* ipv4 only */
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, PORT, &hints, &servinfo) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    /* Loop until we find the first addrinfo we can bind to */
    struct addrinfo *curr;
    for (curr = servinfo; curr != NULL; curr = curr->ai_next) {
        /* get a new socket to listen on */
        if ((servfd = socket(curr->ai_family, curr->ai_socktype,
                          curr->ai_protocol)) < 0) {
            perror("[SERVER] socket");
            continue;
        }

        /* free up the socket before bind in case kernel still hasn't done that */
        if ((setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof &yes)) < 0) {
            perror("[SERVER] setsockopt");
            exit(1);
        }

        if (bind(servfd, curr->ai_addr, curr->ai_addrlen) < 0) {
            perror("[SERVER] bind");
            continue;
        }

        break;
    }

    if (curr == NULL) {
        fputs("[SERVER] failed to bind\n", stderr);
        exit(1);
    }

    free(servinfo);

    if (listen(servfd, BACKLOG) < 0) {
       perror("[SERVER] listen");
       exit(1);
    }

    return servfd;
}

int main(int argc, char **argv)
{
    struct sockaddr_storage remoteaddr;
    socklen_t remoteaddr_size;

    ROOT_DIR = getenv("PWD"); /* default, might get overwritten by parse_args */
    parse_args(argc, argv);

    if (ROOT_DIR == NULL) {
        fputs("[SERVER] ROOT_DIR is null."
              "Reading env var 'PWD' failed and/or specified arg is invalid.\n",
              stderr);
        exit(1);
    }

    printf("[SERVER] initializing with ROOT_DIR '%s'\n", ROOT_DIR);

    char addrbuff[INET6_ADDRSTRLEN], recvbuff[DATA_CAP];
    int servfd, client_fd;

    if (!strncmp(ROOT_DIR, "_default_", 9)) {
        fputs("[SERVER] Config error: the ROOT_DIR is set to '_default_', "
              "please change it and recompile.\n",
              stderr);
        exit(1);
    }

    servfd = setup_local_socket();
    puts("[SERVER] ready to accept connections");

    for (;;) {
        remoteaddr_size = sizeof(remoteaddr);
        client_fd = accept(servfd,
                           (struct sockaddr*)&remoteaddr, &remoteaddr_size);

        if (client_fd < 0) {
            perror("[SERVER] accept");
            continue;
        }

        inet_ntop(remoteaddr.ss_family,
                  &((struct sockaddr_in*)&remoteaddr)->sin_addr,
                  addrbuff, remoteaddr_size);

        printf("[SERVER] got connection from %s\n", addrbuff);

        int bytes_recv = recv(client_fd, recvbuff, DATA_CAP-1, 0);
        if (bytes_recv < 0) {
            perror("recv");
            shutdown(client_fd, 2);
            continue;
        }

        recvbuff[bytes_recv] = '\0';
        printf("[CLIENT]\n%s\n", recvbuff);

        char *response = generate_response(recvbuff);
        /* TODO: make sure entire response is sent */
        int bytes_sent = send(client_fd, response, strlen(response), 0);
        if (bytes_sent < 0) {
          perror("send");
          return -1;
        }

        printf("[RESPONSE]\n%s\n", response);
        free(response);

        shutdown(client_fd, 2);
    }
}
