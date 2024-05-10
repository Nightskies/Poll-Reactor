#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

struct event_handler
{
	void *instance;
	int fd;
	void (*send_callback)(struct event_handler *handler);
	void (*recv_callback)(struct event_handler *handler);
};

#endif // EVENT_HANDLER_H