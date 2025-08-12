#ifndef MEMORY_CONNECTION_H
#define MEMORY_CONNECTION_H

#include <commons/config.h>
typedef enum {
    MEM_OK   = 0,
    MEM_FAIL = 1
} mem_response_t;

// Inicializa ip/puerto de Memoria (debe llamarse una sola vez en main)
void connect_to_memory(t_config* config);

// Pide init de proceso: abre socket, envía PID+size, recibe OK/FAIL, cierra
mem_response_t memory_request_initialize(int pid, int process_size);

// Notifica terminate: abre socket, envía PID, recibe OK/FAIL, cierra
mem_response_t memory_request_terminate(int pid);

#endif // MEMORY_CONNECTION_H
