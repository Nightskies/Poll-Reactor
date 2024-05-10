#include "server.h"
#include "reactor.h"
#include "error.h"

#include <getopt.h>
#include <signal.h>

#define MAX_CLIENTS 64

static struct server *server;

void interrupt_handler(int sig)
{
    (void)sig;

    server_destroy(server);
}

const char *sopts = "hp:";

int main(int argc, char *argv[])
{
    int res;
    unsigned short port = 0;

    while ((res = getopt(argc, argv, sopts)) != -1) {
        switch (res)
        {
        case 'h':
            printf("Usage: %s -p [port]", argv[0]);
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case '?':
        default:
            printf("Usage: %s -p [port]", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (port == 0) {
        printf("Usage: %s -p [port]", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, interrupt_handler) == SIG_ERR) {
        SYS_ERROR("signal");
    }

    server = server_create(port, MAX_CLIENTS);

    for (;;) {
        reactor_handle();
    }

    return 0;
}