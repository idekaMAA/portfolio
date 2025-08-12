
#ifndef DISPATCH_LISTENER_H
#define DISPATCH_LISTENER_H
#include <commons/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <connection/connection.h>
#include <config/config.h> 
#include <handlers/dispatch_handler.h>


void* dispatch_listener(void* config_ptr);
#endif