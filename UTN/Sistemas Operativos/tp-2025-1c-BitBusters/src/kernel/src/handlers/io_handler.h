#define HANDLE_IO_H

#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <syscalls/io_syscall.h>


void init_io_connections();
void register_io(const char* device_name, int socket);
int get_io_socket(const char* device_name);
void syscall_to_io(t_syscall_io* syscall);
