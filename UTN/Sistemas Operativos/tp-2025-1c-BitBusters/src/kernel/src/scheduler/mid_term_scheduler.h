#ifndef MID_TERM_SCHEDULER_H
#define MID_TERM_SCHEDULER_H

#include <context/k_context.h>
#include <connections/memory_connection.h>
#include <config/config.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <connection/connection.h>

void* mid_term_scheduler(void* ctx_ptr);


#endif // MID_TERM_SCHEDULER_H