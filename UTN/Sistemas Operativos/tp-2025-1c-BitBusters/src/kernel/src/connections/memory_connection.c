#include "memory_connection.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <config/config.h>
#include <connection/connection.h>

extern t_log* logger;

// Variables internas para guardar ip/puerto
static char* memory_ip   = NULL;
static char* memory_port = NULL;

/**
 * Llama a create_connection() y guarda los datos para futuros requests.
 */
void connect_to_memory(t_config* config) {
    const char* ip   = search_ip(config,   "IP_MEMORY");
    const char* port = search_port(config, "PORT_MEMORY");

    // Guardamos copias
    memory_ip   = strdup(ip);
    memory_port = strdup(port);

    // Test de conexión (opcional)
    int sock = create_connection(memory_ip, memory_port);
    if (sock < 0) {
        log_error(logger, "Memory: no puede conectar a %s:%s",
                  memory_ip, memory_port);
        return;
    }
    log_info(logger, "Memory: conectado a %s:%s", memory_ip, memory_port);
    close(sock);
}

/**
 * Interna: abre socket, empaqueta un op_code y argumentos (ints en network order),
 * envía, recibe 1 byte de respuesta (0=OK,1=FAIL), cierra socket.
 */
static mem_response_t memory_send_request(int8_t op_code,
                                          int32_t* args,
                                          size_t  nargs) {
    // 1) Abrir conexión
    int sock = create_connection(memory_ip, memory_port);
    if (sock < 0) {
        log_error(logger, "Memory: fallo al abrir socket para op %d", op_code);
        return MEM_FAIL;
    }

    // 2) Empaquetar mensaje: [1 byte op] [nargs x 4 bytes args]
    size_t  msg_size = 1 + nargs * sizeof(int32_t);
    uint8_t* buf     = malloc(msg_size);
    buf[0] = (uint8_t)op_code;
    for (size_t i = 0; i < nargs; i++) {
        int32_t net = htonl(args[i]);
        memcpy(buf + 1 + i * 4, &net, 4);
    }

    // 3) Enviar y liberar buffer
    if (send(sock, buf, msg_size, 0) != msg_size) {
        log_error(logger, "Memory: error enviando request op %d", op_code);
        free(buf);
        close(sock);
        return MEM_FAIL;
    }
    free(buf);

    // 4) Recibir 1 byte de respuesta
    uint8_t resp;
    if (recv(sock, &resp, 1, MSG_WAITALL) != 1) {
        log_error(logger, "Memory: no llegó respuesta para op %d", op_code);
        close(sock);
        return MEM_FAIL;
    }

    // 5) Cerrar socket y retornar
    close(sock);
    return (resp == 0) ? MEM_OK : MEM_FAIL;
}

mem_response_t memory_request_initialize(int pid, int process_size) {
    int32_t args[2] = { pid, process_size };
    return memory_send_request(1, args, 2);
}

mem_response_t memory_request_terminate(int pid) {
    int32_t args[1] = { pid };
    return memory_send_request(2, args, 1);
}
