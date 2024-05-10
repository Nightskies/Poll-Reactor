#include "reactor.h"
#include "error.h"

#include <poll.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_NO_OF_HANDLES 32

#ifndef INFTIM
#define INFTIM -1
#endif

struct poll_observer
{
	struct event_handler handler;
	struct pollfd poll;
	bool is_used;
};

static struct poll_observer registered_handlers[MAX_NO_OF_HANDLES];

static size_t build_poll_array(struct pollfd *fds);

static struct event_handler *find_handler(int fd);

static void dispatch_events(const struct pollfd* fds, size_t noOfHandles);

void reactor_register(struct event_handler *handler)
{  
	bool is_registered = false;
	for (int i = 0; (i < MAX_NO_OF_HANDLES) && (is_registered == false); ++i) {   
		if (registered_handlers[i].is_used == false) {
        	struct poll_observer *free_entry = &registered_handlers[i];
         	free_entry->handler = *handler;
			free_entry->poll.fd = handler->fd;
			free_entry->poll.events = POLLIN | POLLOUT;

         	is_registered = free_entry->is_used = true;
         
        	printf("Reactor: Added handle with ID = %d\n", free_entry->poll.fd);
      	}
   	}

	if (is_registered == false) {
		ERROR("no more free slot for client");
	}
}

void reactor_unregister(struct event_handler *handler)
{
   	bool is_unregistered = false;
   
   	for (int i = 0; (i < MAX_NO_OF_HANDLES) && (is_unregistered == false); ++i) {
      	if (registered_handlers[i].is_used && (registered_handlers[i].handler.instance == handler->instance)) {
        	registered_handlers[i].is_used = false;
		 	is_unregistered = true;
         
			printf("Reactor: Removed event handler with ID = %d\n", registered_handlers[i].poll.fd);
      	}
   	}

	if (is_unregistered == false) {
		ERROR("can't unregistered client");
	}
}


void reactor_handle(void)
{
   	struct pollfd fds[MAX_NO_OF_HANDLES] = {0};

	const size_t no_of_handles = build_poll_array(fds);

   	if (poll(fds, no_of_handles, INFTIM) < 0) {
		SYS_ERROR("poll");
    } else {
    	dispatch_events(fds, no_of_handles);
   	}
}

static size_t build_poll_array(struct pollfd *fds)
{   
	int i;
   	for (i = 0; i < MAX_NO_OF_HANDLES; ++i) {
    	if (registered_handlers[i].is_used) {
        	fds[i] = registered_handlers[i].poll;
      	}
   	}
   
   	return i;
}

static struct event_handler *find_handler(int fd)
{
   	struct event_handler *matching_handler = NULL;
   
	for (int i = 0; (i < MAX_NO_OF_HANDLES) && (matching_handler == NULL); ++i) {
    	if (registered_handlers[i].is_used && (registered_handlers[i].poll.fd == fd)) {
        	matching_handler = &registered_handlers[i].handler;
      	}
   	}
   
   	return matching_handler;
}

static void dispatch_events(const struct pollfd *fds, size_t no_of_handles)
{
	struct event_handler *handler = NULL;
   	for (size_t i = 0; i < no_of_handles; ++i) {
		handler = find_handler(fds[i].fd);
		if (handler == NULL) {
			continue;
		}
		if (POLLHUP & fds[i].revents) {
			
		}
		if (POLLERR & fds[i].revents) {
		
      	}
		if (POLLIN & fds[i].revents) {
			if (handler->recv_callback != NULL) {
				handler->recv_callback(handler);
			}
		}
		if (POLLOUT & fds[i].revents) {
			if (handler->send_callback != NULL) {
				handler->send_callback(handler);
			}
		}
   	}
}