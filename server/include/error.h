#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ERROR(msg) \
    fprintf(stderr, "%s:%d fatal error: %s\n", __FILE__, __LINE__, msg); \
    exit(EXIT_FAILURE)

#define SYS_ERROR(msg) \
    fprintf(stderr, "%s:%d fatal error %s:%s\n", __FILE__, __LINE__, msg, strerror(errno)); \
    exit(EXIT_FAILURE)

#endif // ERROR_H