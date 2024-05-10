#ifndef SERVER_H
#define SERVER_H

struct server;

struct server *server_create(unsigned short port, unsigned int max_clients);

void server_destroy(struct server *server);

#endif // SERVER_H