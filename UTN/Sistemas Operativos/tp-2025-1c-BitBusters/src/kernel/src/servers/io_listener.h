#define IO_LISTENER_H

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <commons/log.h>
#include <config/config.h>
#include <connection/connection.h>
#include <handlers/io_handler.h>


void* io_listener(void* ctx_ptr);
void* handle_io_device(void* arg); 