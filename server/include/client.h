#ifndef CLIENT_H
#define CLIENT_H

#include "server_notifier.h"

struct client;

struct client *client_create(int server_fd, const struct server_notifier *notifier);

void client_destroy(struct client *client);

#endif // CLIENT_H