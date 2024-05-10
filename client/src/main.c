#include "error.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

static char *sopts = "p:a:";

int main(int argc, char *argv[])
{
    int res;
    struct sockaddr_in peer_addr;
	memset(&peer_addr, 0, sizeof(peer_addr));
	peer_addr.sin_family = AF_INET;

    while ((res = getopt(argc, argv, sopts)) != -1) {
        switch (res)
        {
        case 'p':
            peer_addr.sin_port = htons(atoi(optarg));
            break;
        case 'a':
            if (inet_aton(optarg, &peer_addr.sin_addr) == 0) {
                SYS_ERROR("inet_aton");
            }
            break;
        case '?':
        default:
            fprintf(stderr, "Usage: %s -a [addr] -p [port]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (peer_addr.sin_port == 0 || peer_addr.sin_addr.s_addr == 0) {
        fprintf(stderr, "Usage: %s -a [addr] -p [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        SYS_ERROR("socket");
    }

    socklen_t len = sizeof(struct sockaddr_in);

    if (connect(fd, (struct sockaddr *)&peer_addr, len) < 0) {
        SYS_ERROR("connect");
    }

    char sbuf[BUFSIZ] = {0};
    char rbuf[BUFSIZ] = {0};
    for (;;) {
        fputs("Enter msg:", stdout);
        if (fgets(sbuf, BUFSIZ, stdin) == NULL) {
            SYS_ERROR("fgets");
        }
        sbuf[strlen(sbuf) - 1] = '\0';

        ssize_t sbytes = send(fd, sbuf, strlen(sbuf), 0);
        if (sbytes < 0) {
            SYS_ERROR("send");
        }
        printf("Sent msg:%s\n", sbuf);

        ssize_t rbytes = recv(fd, rbuf, BUFSIZ, 0);
        if (rbytes < 0) {
            SYS_ERROR("recv");
        } else if (rbytes == 0) {
            printf("Server quit\n");
            break;
        }
        printf("Recv msg:%s\n", rbuf);

        memset(sbuf, 0, BUFSIZ);
        memset(rbuf, 0, BUFSIZ);
    }
    
    close(fd);

    return 0;
}