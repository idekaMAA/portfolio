#define INTERRUPT_LISTENER_H
#include <commons/log.h>
#include <config/config.h>
#include <unistd.h>
#include <connection/connection.h>
#include <syscalls/io_syscall.h>
#include <handlers/io_handler.h>
#include <context/k_context.h>


void* interrupt_listener(void* kernel_context_ptr);