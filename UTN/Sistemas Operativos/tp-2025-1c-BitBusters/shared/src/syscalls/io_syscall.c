#include "io_syscall.h"
#include <stdlib.h>
#include <string.h>

t_syscall_io* syscall_io_create(int pid, const char* device, int time) {
    t_syscall_io* io = malloc(sizeof(t_syscall_io));
    io->pid = pid;
    io->time = time;
    strncpy(io->device, device, sizeof(io->device) - 1);
    io->device[sizeof(io->device) - 1] = '\0';
    return io;
}

void syscall_io_destroy(t_syscall_io* io) {
    free(io);
}

void* serialize_syscall_io(t_syscall_io* io, int* size) {
    *size = sizeof(int) * 2 + sizeof(io->device);

    void* buffer = malloc(*size);
    int offset = 0;

    memcpy(buffer + offset, &io->pid, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, &io->time, sizeof(int));
    offset += sizeof(int);
    memcpy(buffer + offset, io->device, sizeof(io->device)); 

    return buffer;
}


t_syscall_io* deserialize_syscall_io(void* buffer, int buffer_size) {
    int offset = 0;
    t_syscall_io* io = malloc(sizeof(t_syscall_io));

    memcpy(&io->pid, buffer + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&io->time, buffer + offset, sizeof(int));
    offset += sizeof(int);

    int device_len = buffer_size - offset;
    memcpy(io->device, buffer + offset, device_len);
    io->device[device_len - 1] = '\0';

    return io;
}
