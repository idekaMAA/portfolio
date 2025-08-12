#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H

#include <commons/log.h>
#include <connection/connection.h>

int connect_to_memory(const char* ip, const char* port);
int connect_to_kernel_dispatch(const char* ip, const char* port);
int connect_to_kernel_interrupt(const char* ip, const char* port);

#endif
