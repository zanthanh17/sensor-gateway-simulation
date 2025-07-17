#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#include "shared_data.h"

typedef struct {
    shared_data_t *shared_data;
    int port;
} connection_manager_args_t;

void *connection_manager_thread(void *arg);
#endif
