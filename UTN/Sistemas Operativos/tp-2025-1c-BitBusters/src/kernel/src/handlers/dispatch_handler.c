// dispatch_handler.c

#include "dispatch_handler.h"
#include <syscalls/io_syscall.h>
#include <handlers/io_handler.h>
#include <context/k_context.h>
#include <structs/pcb/pcb.h>
#include <scheduler/long_term_scheduler.h>
#include <scheduler/short_term_scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

extern t_log* logger;

#define INIT_PROC 1
#define EXIT_PROC 2
#define IO_PROC   3


void* handle_dispatch_client(void* arg_ptr) {
    dispatch_arg_t* arg = (dispatch_arg_t*)arg_ptr;
    int sock = arg->socket;
    kernel_context_t* ctx = arg->ctx;
    free(arg);

    log_info(ctx->logger, "Dispatch: CPU conectada en socket %d", sock);

    while (true) {
        // 1) Leer código de mensaje (4 bytes, en red)
        int code_net;
        int ret = recv(sock, &code_net, sizeof(code_net), MSG_WAITALL);
        if (ret <= 0) {
            log_warning(ctx->logger, "Dispatch: conexión cerrada o error recv()");
            break;
        }
        int code = ntohl(code_net);
        if (code == INIT_PROC) {
            // --- INIT_PROC: parámetros (ruta + tamaño) ---
            int path_len_net;
            recv(sock, &path_len_net, sizeof(path_len_net), MSG_WAITALL);
            int path_len = ntohl(path_len_net);

            char* path = malloc(path_len + 1);
            recv(sock, path, path_len, MSG_WAITALL);
            path[path_len] = '\0';

            int size_net;
            recv(sock, &size_net, sizeof(size_net), MSG_WAITALL);
            int proc_size = ntohl(size_net);

            // Crear PCB y encolarlo en NEW
            t_pcb* pcb = create_pcb(generate_pid(), proc_size);
            // pcb->script_path = strdup(path);

            pthread_mutex_lock(&ctx->new_mutex);
            list_add(ctx->new_queue, pcb);
            pthread_cond_signal(&ctx->new_cond);
            pthread_mutex_unlock(&ctx->new_mutex);

            log_info(ctx->logger,
                     "## (%d) Se crea el proceso - Estado: NEW (INIT %s, size=%d)",
                     pcb->pid, path, proc_size);

            free(path);
            continue;  // volver a leer el siguiente mensaje
        }
        else if (code == EXIT_PROC) {
            // --- EXIT_PROC: no recibe parámetros ---
            // Procesar finalización inmediata
            // (asumimos que el PID actual está en la PCB que se despachó)
            // Aquí podrías encolar una señal a LP o directamente liberar
            // Para simplificar, solo logueamos:
            log_info(ctx->logger, "Dispatch: EXIT_PROC recibido");
            // el LP/dispatcher principal se encargará de liberar y signal_memory_freed()
            continue;
        }
        else if (code == IO_PROC) {
            // --- IO_PROC: parámetros (device + time) ---
            int dev_len_net;
            recv(sock, &dev_len_net, sizeof(dev_len_net), MSG_WAITALL);
            int dev_len = ntohl(dev_len_net);
            char* device = malloc(dev_len + 1);
            recv(sock, device, dev_len, MSG_WAITALL);
            device[dev_len] = '\0';
        
            int time_net;
            recv(sock, &time_net, sizeof(time_net), MSG_WAITALL);
            int io_time = ntohl(time_net);
        
            // 1) Muevo el PCB que estaba en EXEC a BLOCKED
            pthread_mutex_lock(&ctx->running_mutex);
            t_pcb* pcb = ctx->current_running;
            pthread_mutex_unlock(&ctx->running_mutex);
        
            // Registro cuándo empezó a bloquearse
            gettimeofday(&pcb->blocked_time, NULL);
        
            pthread_mutex_lock(&ctx->blocked_mutex);
            list_add(ctx->blocked_queue, pcb);
            pthread_mutex_unlock(&ctx->blocked_mutex);
        
            log_info(ctx->logger,
                     "## (%d) - Pasa a BLOCKED por IO %s %d",
                     pcb->pid, device, io_time);
        
            // 2) Enviar la syscall IO al módulo IO correspondiente
            t_syscall_io* sys = syscall_io_create(pcb->pid, device, io_time);
            syscall_to_io(sys);
            syscall_io_destroy(sys);
        
            free(device);
            continue;
        }
        
        // --- Si no es syscall, entonces es turno de dispatch normal ---
        // 2) Esperar PCB en dispatch_queue
        pthread_mutex_lock(&ctx->dispatch_mutex);
        while (list_is_empty(ctx->dispatch_queue)) {
            pthread_cond_wait(&ctx->dispatch_cond, &ctx->dispatch_mutex);
        }
        t_pcb* to_dispatch = list_remove(ctx->dispatch_queue, 0);
        pthread_mutex_unlock(&ctx->dispatch_mutex);

        // 3) Métricas y log de transición
        add_state_metric(to_dispatch, "EXEC");
        log_info(ctx->logger, "## (%d) Pasa a EXEC", to_dispatch->pid);

        // 4) Enviar PID+PC al CPU
        k_message* msg = context_create(to_dispatch->pid, to_dispatch->pc);
        int buf_size;
        void* buffer = context_serialize(msg, &buf_size);
        if (send(sock, buffer, buf_size, 0) != buf_size) {
            log_error(ctx->logger,
                      "Dispatch: error enviando PCB %d al CPU", to_dispatch->pid);
        } else {
            log_info(ctx->logger,
                     "Dispatch: enviado PID=%d PC=%d",
                     msg->pid, msg->pc);
        }

        pthread_mutex_lock(&ctx->running_mutex);
        ctx->current_running = to_dispatch;
        ctx->current_cpu_fd  = sock;
        pthread_mutex_unlock(&ctx->running_mutex);

        free(buffer);
        context_destroy(msg);

        // 5) Recibir PID y motivo de vuelta (8 bytes: PID + motivo)
        int resp_buf[2];
        int received = 0;
        while (received < sizeof(resp_buf)) {
            int r = recv(sock,
                         ((char*)resp_buf) + received,
                         sizeof(resp_buf) - received,
                         0);
            if (r <= 0) {
                log_error(ctx->logger,
                          "Dispatch: error recv() de PID/motivo");
                break;
            }
            received += r;
        }
        int pid_ret  = ntohl(resp_buf[0]);
        int reason   = ntohl(resp_buf[1]); // 0=terminó ráfaga,1=bloqueo,2=exit

        // 6) Procesar motivo
        switch (reason) {
            case 0:
                log_info(ctx->logger,
                         "## (%d) - Fin de ráfaga", pid_ret);
                // actualizar ráfaga real y reencolar en READY si corresponde
                break;
            case 1:
                log_info(ctx->logger,
                         "## (%d) - Bloqueado por IO", pid_ret);
                break;
            case 2:
                log_info(ctx->logger,
                         "## (%d) - Finaliza proceso", pid_ret);
                // informar a Memoria y liberar PCB
                signal_memory_freed(ctx);
                break;
            default:
                log_warning(ctx->logger,
                            "Dispatch: motivo desconocido %d para PID %d",
                            reason, pid_ret);
        }

        // Volver a esperar en el while(true)
    }

    close(sock);
    return NULL;
}
