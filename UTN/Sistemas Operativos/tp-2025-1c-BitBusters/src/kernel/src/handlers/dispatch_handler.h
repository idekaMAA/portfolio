
#ifndef DISPATCH_HANDLER_H
#define DISPATCH_HANDLER_H

#include <commons/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <structs/pcb/pcb.h>
#include <sys/socket.h>
#include <pthread.h>
#include <context/k_message.h>
#include <context/k_context.h>
#include <commons/config.h>

void* handle_dispatch_client(void* client_socket_ptr);

typedef struct {
    int socket;
    kernel_context_t* ctx;
} dispatch_arg_t;
#endif 