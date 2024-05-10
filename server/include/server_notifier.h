#ifndef SERVER_NOTIFIER_H
#define SERVER_NOTIFIER_H

struct server;
struct client;

struct server_notifier
{
	struct server *server;
 
   	void (*client_on_closed)(struct server *server, struct client *client);
};

#endif // SERVER_NOTIFIER_H