#include "init_proc.h"

#define INIT_PROC_CODE 1

void INIT_PROC(int socket_dispatch, int proc_size) {

    int32_t code_n = htonl(1);
    send(socket_dispatch, &code_n, sizeof(code_n), 0);

    int32_t size_n = htonl(proc_size);
    send(socket_dispatch, &size_n, sizeof(size_n), 0);
}

