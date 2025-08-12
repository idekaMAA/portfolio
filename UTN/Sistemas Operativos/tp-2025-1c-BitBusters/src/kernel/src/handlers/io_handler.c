#include "io_handler.h"
#include "../../shared/src/syscalls/io_syscall.h"

extern t_log* logger;

static t_dictionary* io_connections = NULL;

void init_io_connections() {
    if (!io_connections) {
        io_connections = dictionary_create();
        log_info(logger, "Registro de conexiones IO inicializadas");
    }
}

void register_io(const char* device_name, int socket) {
    if (!io_connections) init_io_connections();

    char* key = strdup(device_name);
    dictionary_put(io_connections, key, (void*)(intptr_t)socket);

    log_info(logger, "Dispositivo IO '%s' registrado con socket %d", device_name, socket);
}

int get_io_socket(const char* device_name) {
    if (!io_connections || !dictionary_has_key(io_connections, (char*)device_name)) {
        log_error(logger, "Dispositivo IO no encontrado: %s", device_name);
        return -1;
    }

    return (int)(intptr_t) dictionary_get(io_connections, (char*)device_name);
}

void syscall_to_io(t_syscall_io* syscall) {
    int socket = get_io_socket(syscall->device);
    if (socket == -1) {
        log_error(logger, "Dispositivo IO no encontrado: %s", syscall->device);
        return;
    }

    int size;
    void* buffer = serialize_syscall_io(syscall, &size);

    if (send(socket, buffer, size, 0) == -1) {
        log_error(logger, "Error al enviar syscall IO a %s", syscall->device);
    } else {
        log_info(logger, "Syscall IO enviada a %s (PID=%d, tiempo=%d ms)",
                 syscall->device, syscall->pid, syscall->time);
    }
    log_info(logger, "## (%d) - Bloqueado por IO: %s", syscall->pid, syscall->device);
    free(buffer);
}
