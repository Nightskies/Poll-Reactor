#include "server.h"
#include "client.h"
#include "reactor.h"
#include "error.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MSG_MAX_SIZE 1024

struct client
{
	struct event_handler handler;
   	struct server_notifier notifier;
   	int fd;
}; 

static int client_accept(int server_fd);
static void client_read(struct event_handler *handler);
static void client_send(struct event_handler *handler);

struct client* client_create(int server_fd, const struct server_notifier *notifier)
{
   	struct client* client = malloc(sizeof(struct client));
   	if (client == NULL) {
    	SYS_ERROR("malloc");
   	}
   
   	client->fd = client_accept(server_fd);
      
	client->handler.instance = client;
   	client->handler.fd = client->fd;
   	client->handler.recv_callback = client_read;
   	client->handler.send_callback = NULL;

   	reactor_register(&client->handler);

   	client->notifier = *notifier;
   
   	return client;
}

void client_destroy(struct client *client)
{
	reactor_unregister(&client->handler);
   
	close(client->fd);

   	free(client);
}

static int client_accept(int server_fd)
{
   	struct sockaddr_in address = {0};
   	socklen_t addr_len = sizeof(struct sockaddr_in);

   	const int fd = accept(server_fd, (struct sockaddr*)&address, &addr_len);   
   	if (fd < 0) {
      	SYS_ERROR("accept");
   	}
   
   	printf("Client: New connection created on IP-address %s\n", inet_ntoa(address.sin_addr));
   
   	return fd;
}

static void client_read(struct event_handler *handler)
{
   	struct client *client = (struct client *)handler->instance;
  
   	char msg[MSG_MAX_SIZE] = {0};
  
   	const ssize_t rbytes = recv(client->fd, msg, MSG_MAX_SIZE, 0);
   	if (rbytes == 0) {
    	client->notifier.client_on_closed(client->notifier.server, client);
		printf("Client with ID %d closed\n", client->fd);
   	} else if (rbytes < 0) {
		SYS_ERROR("recv");
	} else {
		printf("Client: received msg:%s\n", msg);
		handler->send_callback = client_send;
	}
}

static void client_send(struct event_handler *handler)
{
   	struct client *client = (struct client *)handler->instance;
  
   	char msg[MSG_MAX_SIZE] = {0};
   	strcpy(msg, "Hi doggy");
  
   	const ssize_t sbytes = send(client->fd, msg, MSG_MAX_SIZE, 0);
   	if (sbytes < 0) {
		SYS_ERROR("send");
   	}

   	printf("Client: sent msg:%s\n", msg);

	handler->send_callback = NULL;
}