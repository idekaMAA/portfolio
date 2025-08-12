// interrupt_listener.c

#include "interrupt_listener.h"
#include <context/k_context.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

extern t_log* logger;

void* interrupt_listener(void* ctx_ptr) {
    kernel_context_t* ctx = (kernel_context_t*) ctx_ptr;
    const char* port = search_port(ctx->config, "PUERTO_ESCUCHA_INTERRUPT");
    int server_socket = create_server(port);

    if (server_socket == -1) {
        log_error(logger, "Interrupt Listener: no se pudo iniciar en puerto %s", port);
        return NULL;
    }

    log_info(logger, "Interrupt Listener: escuchando en puerto %s", port);
    sem_post(&ctx->interrupt_ready);

    while (1) {
        int cpu_fd = accept(server_socket, NULL, NULL);
        if (cpu_fd == -1) {
            log_error(logger, "Interrupt Listener: accept falló");
            continue;
        }

        // Registro el socket de CPU para interrupciones futuras
        pthread_mutex_lock(&ctx->interrupt_mutex);
        list_add(ctx->interrupt_sockets, (void*)(intptr_t)cpu_fd);
        pthread_mutex_unlock(&ctx->interrupt_mutex);

        log_info(logger, "Interrupt Listener: CPU conectada en socket %d", cpu_fd);
        // No cerramos cpu_fd: lo usaremos para send_interrupt()
    }

    // nunca llega acá
    // close(server_socket);
    return NULL;
}
