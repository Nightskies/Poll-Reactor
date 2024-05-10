#include "server_notifier.h"
#include "reactor.h"
#include "client.h"
#include "error.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct server
{
	int listen_socket;
   	unsigned int max_clients;
   	struct event_handler handler;
   	struct client **clients;
};

static int find_free_slot(const struct server *server);
static int find_occup_slot(const struct server *server, const struct client *client);
static int find_slot(const struct server *server, const struct client *client);

static void server_connect_request(struct event_handler *handler);

static void server_remove_client(struct server *server, struct client *client);

struct server *server_create(unsigned short port, unsigned int max_clients)
{
	struct server *server = malloc(sizeof(struct server));
   	if (server == NULL) {
		SYS_ERROR("malloc");
   	}

   	server->clients = (struct client **)malloc(max_clients * sizeof(struct client *));
   	if (server->clients == NULL) {
		SYS_ERROR("malloc");
   	}

   	for (unsigned int i = 0; i < max_clients; ++i) {
    	server->clients[i] = NULL;
   	}

	const int fd = socket(AF_INET, SOCK_STREAM, 0);
   	if (fd < 0) {
		SYS_ERROR("socket");
   	}
   
   	struct sockaddr_in address = {0};
   	address.sin_family = AF_INET;
   	address.sin_port = htons(port);
   	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0) {
		SYS_ERROR("bind");
	}

	if (listen(fd, 10) < 0) {
		SYS_ERROR("listen");
	}

	printf("Server created at %s address and %d port\n", inet_ntoa(address.sin_addr), htons(port));

   	server->listen_socket = fd;
   	server->max_clients = max_clients;
	server->handler.fd = fd;
   	server->handler.instance = server;
   	server->handler.recv_callback = server_connect_request;
	server->handler.send_callback = NULL;

   	reactor_register(&server->handler);
      
   	return server;
}

void server_destroy(struct server *server)
{
	for (unsigned int i = 0; i < server->max_clients; ++i) {
    	if (server->clients[i] != NULL) {
        	client_destroy(server->clients[i]);
      	}
   	}

   	reactor_unregister(&server->handler);

   	close(server->listen_socket);

   	free(server);
}

static void server_connect_request(struct event_handler *handler)
{
   	struct server *server = (struct server *)handler->instance;
  
   	const int free_slot = find_free_slot(server); 
   	if (free_slot < 0) {
    	printf("Server: Not space for more clients\n");
   	}

	struct server_notifier notifier = {0};
   	notifier.server = server;
   	notifier.client_on_closed = server_remove_client;
      
	server->clients[free_slot] = client_create(server->listen_socket, &notifier);
        
	printf("Server: Incoming connect request accepted\n");
}

static void server_remove_client(struct server *server, struct client *client)
{
	const int client_slot = find_occup_slot(server, client);
   	if (client_slot < 0) {
    	ERROR("phantom client detected");
   	}

	client_destroy(client);

   	server->clients[client_slot] = NULL;
}

static int find_slot(const struct server *server, const struct client *client)
{
	int slot = -1;
      
	for (unsigned int i = 0; i < server->max_clients; ++i) {
      	if (client == server->clients[i]) {
    		slot = i;
      	}
   	}
   
	return slot;
}

static int find_free_slot(const struct server *server)
{
	return find_slot(server, NULL);
}

static int find_occup_slot(const struct server *server, const struct client *client)
{  
	return find_slot(server, client);
}