#include "connection_cpu.h"

extern t_log* logger;  // Declarar logger externo para usar en logs

int connect_to_memory(const char* ip, const char* port) {
    int socket = create_connection(ip, port);
    if (socket == -1) {
        log_error(logger, "No se pudo conectar al servidor de memoria");
    } else {
        log_info(logger, "Conectado con memoria en %s:%s", ip, port);
    }
    return socket;
}

int connect_to_kernel_dispatch(const char* ip, const char* port) {
    int socket = create_connection(ip, port);
    if (socket == -1) {
        log_error(logger, "No se pudo conectar al kernel (dispatch)");
    } else {
        log_info(logger, "Conectado con kernel dispatch en %s:%s", ip, port);
    }
    return socket;
}

int connect_to_kernel_interrupt(const char* ip, const char* port) {
    int socket = create_connection(ip, port);
    if (socket == -1) {
        log_error(logger, "No se pudo conectar al kernel (interrupt)");
    } else {
        log_info(logger, "Conectado con kernel interrupt en %s:%s", ip, port);
    }
    return socket;
}
