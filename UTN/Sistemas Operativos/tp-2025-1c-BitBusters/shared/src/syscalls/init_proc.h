#ifndef INIT_SYSCALL_H
#define INIT_SYSCALL_H

#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/**
 * @brief Código de syscall para inicializar un proceso
 */
#define INIT_PROC_CODE 1
/**
 * @brief Envía una solicitud de inicialización de proceso al dispatcher
 * 
 * @param socket_dispatch El socket del dispatcher
 * @param proc_size El tamaño del proceso
 */
void INIT_PROC(int socket_dispatch, int proc_size);
#endif