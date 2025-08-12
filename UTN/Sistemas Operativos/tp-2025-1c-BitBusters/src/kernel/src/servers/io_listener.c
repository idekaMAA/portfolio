
#include "io_listener.h"
#include <context/k_context.h>
#include <handlers/io_handler.h>     // register_io, get_io_socket, syscall_to_io
#include <structs/pcb/pcb.h>                 // find_pcb_by_pid (ver nota)
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

extern t_log* logger;

// Argumento para el hilo de cada conexión IO
typedef struct {
    int socket;
    kernel_context_t* ctx;
} io_arg_t;

// Helper: busca un PCB bloqueado por PID en la lista de BLOCKED y lo remueve
static t_pcb* pop_blocked_pcb(kernel_context_t* ctx, int pid) {
    // Asumimos que tienes una lista ctx->blocked_queue con los PCBs en BLOCKED.
    // Si no existe, deberías crearla e insertar ahí cada PCB al llamar a syscall_to_io().
    for (int i = 0; i < list_size(ctx->blocked_queue); i++) {
        t_pcb* pcb = list_get(ctx->blocked_queue, i);
        if (pcb->pid == pid) {
            list_remove(ctx->blocked_queue, i);
            return pcb;
        }
    }
    return NULL;
}

// Hilo que atiende a un módulo IO concreto
void* handle_io_device(void* arg) {
    io_arg_t* ioarg = (io_arg_t*)arg;
    int sock = ioarg->socket;
    kernel_context_t* ctx = ioarg->ctx;
    free(ioarg);

    // 1) Recibir nombre del dispositivo (string terminado en '\0')
    char device_name[65];
    int idx = 0;
    while (idx < 64) {
        char c;
        if (recv(sock, &c, 1, MSG_WAITALL) != 1) {
            log_warning(logger, "IO Listener: fallo leyendo nombre de dispositivo");
            close(sock);
            return NULL;
        }
        device_name[idx++] = c;
        if (c == '\0') break;
    }
    device_name[64] = '\0';
    log_info(logger, "IO Listener: módulo '%s' conectado en socket %d", device_name, sock);

    // 2) Registrar la conexión en io_handler
    register_io(device_name, sock);

    // 3) Loop de mensajes FIN_IO
    char buf[32];
    while (1) {
        int bytes = recv(sock, buf, sizeof(buf)-1, 0);
        if (bytes <= 0) {
            log_warning(logger, "IO Listener: desconexión de dispositivo '%s'", device_name);
            break;
        }
        buf[bytes] = '\0';

        // Formato esperado: "FIN_IO <pid>\n" o "FIN_IO<pid>"
        if (strncmp(buf, "FIN_IO", 6) == 0) {
            int pid = atoi(buf + 6);
            log_info(logger, "## (%d) - Finalizó IO '%s'", pid, device_name);

            // 3a) Sacar PCB de blocked_queue
            t_pcb* pcb = pop_blocked_pcb(ctx, pid);
            if (!pcb) {
                log_error(logger, "IO Listener: PCB %d no estaba en BLOCKED", pid);
                continue;
            }

            // 3b) Pasar PCB a READY
            pthread_mutex_lock(&ctx->ready_mutex);
            list_add(ctx->ready_queue, pcb);
            pthread_cond_signal(&ctx->ready_cond);
            pthread_mutex_unlock(&ctx->ready_mutex);

            log_info(logger, "## (%d) - Pasa de BLOCKED a READY", pid);
        }
    }

    close(sock);
    return NULL;
}

void* io_listener(void* ctx_ptr) {
    kernel_context_t* ctx = (kernel_context_t*)ctx_ptr;
    const char* port = search_port(ctx->config, "PUERTO_ESCUCHA_IO");
    int server = create_server(port);
    if (server == -1) {
        log_error(logger, "IO Listener: no pudo abrir puerto %s", port);
        return NULL;
    }

    log_info(logger, "IO Listener: escuchando en puerto %s", port);
    sem_post(&ctx->io_ready);

    // Asegurarse de tener la cola de blocked
    if (!ctx->blocked_queue) {
        ctx->blocked_queue = list_create();
    }

    while (1) {
        int client = accept(server, NULL, NULL);
        if (client == -1) {
            log_error(logger, "IO Listener: accept falló");
            continue;
        }
        // Lanzar un hilo para manejar este módulo IO
        pthread_t thr;
        io_arg_t* arg = malloc(sizeof(io_arg_t));
        arg->socket = client;
        arg->ctx    = ctx;
        pthread_create(&thr, NULL, handle_io_device, arg);
        pthread_detach(thr);
    }
    // nunca llega acá
    return NULL;
}
