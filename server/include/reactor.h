#ifndef REACTOR_H
#define REACTOR_H

#include "event_handler.h"

void reactor_handle(void);

void reactor_register(struct event_handler *handler);

void reactor_unregister(struct event_handler *handler);

#endif // REACTOR_H