// dispatch_listener.c

#include "dispatch_listener.h"
#include "context/k_context.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>

extern t_log* logger;

void* dispatch_listener(void* ctx_ptr) {
    kernel_context_t* ctx = (kernel_context_t*)ctx_ptr;

    const char* port = search_port(ctx->config, "PUERTO_ESCUCHA_DISPATCH");
    int server_socket = create_server(port);
    if (server_socket == -1) {
        log_error(logger,
                  "Dispatch Listener: no pude arrancar en puerto %s",
                  port);
        return NULL;
    }
    log_info(logger, "Dispatch Listener: escuchando en puerto %s", port);
    sem_post(&ctx->dispatch_ready);
    while (true) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client_fd = accept(server_socket,
                               (struct sockaddr*)&addr,
                               &len);
        if (client_fd == -1) {
            log_error(logger, "Dispatch Listener: accept falló");
            continue;
        }

        // Preparamos los argumentos para el hilo
        dispatch_arg_t* arg = malloc(sizeof(dispatch_arg_t));
        arg->socket = client_fd;
        arg->ctx    = ctx;

        pthread_t thread;
        if (pthread_create(&thread,
                           NULL,
                           handle_dispatch_client,
                           arg) != 0) {
            log_error(logger,
                      "Dispatch Listener: no pude crear hilo para socket %d",
                      client_fd);
            close(client_fd);
            free(arg);
            continue;
        }
        pthread_detach(thread);
    }

    // (Opcional) nunca llegamos acá, pero podrías cerrar:
    // close(server_socket);
    return NULL;
}
