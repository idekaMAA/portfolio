#include "mid_term_scheduler.h"
#define OPCODE_SWAP_OUT 2    // asume 2 = SWAP_OUT en memoria
#define RESP_OK         0

void* mid_term_scheduler(void* ctx_ptr) {
    kernel_context_t* ctx = (kernel_context_t*)ctx_ptr;
    log_info(ctx->logger, "Planificador de mediano plazo iniciado.");

    while (true) {
        // Pequeño delay para no busy-loop (por ejemplo cada 100 ms)
        usleep(100 * 1000);

        struct timeval now;
        gettimeofday(&now, NULL);

        pthread_mutex_lock(&ctx->blocked_mutex);
        for (int i = 0; i < list_size(ctx->blocked_queue); ) {
            t_pcb* pcb = list_get(ctx->blocked_queue, i);
            // Calcula cuánto lleva bloqueado (ms)
            long elapsed = (now.tv_sec  - pcb->blocked_time.tv_sec ) * 1000
                         + (now.tv_usec - pcb->blocked_time.tv_usec) / 1000;
            if (elapsed < ctx->suspension_time) {
                i++; // aún no cumple
                continue;
            }

            // 1) Pasar a SUSP.BLOCKED
            list_remove(ctx->blocked_queue, i);
            log_info(ctx->logger,
                     "## (%d) - Pasa a SUSP. BLOCKED tras %ld ms bloqueado",
                     pcb->pid, elapsed);

            // 2) Informar a Memoria para swappear
            const char* ip   = search_ip(ctx->config,   "IP_MEMORY");
            const char* port = search_port(ctx->config, "PORT_MEMORY");
            int mem_fd = create_connection(ip, port);
            if (mem_fd < 0) {
                log_error(ctx->logger,
                          "MP: no pude conectar a Memoria para SWAP_OUT");
            } else {
                uint8_t  opcode = OPCODE_SWAP_OUT;
                uint32_t pid_n  = htonl(pcb->pid);

                send(mem_fd, &opcode, sizeof(opcode), 0);
                send(mem_fd, &pid_n,  sizeof(pid_n),  0);

                uint8_t resp;
                if (recv(mem_fd, &resp, sizeof(resp), MSG_WAITALL) != sizeof(resp)) {
                    log_error(ctx->logger,
                              "MP: falla recv respuesta SWAP_OUT");
                    resp = 1; // forzamos error
                }
                close(mem_fd);

                if (resp != RESP_OK) {
                    log_error(ctx->logger,
                              "MP: SWAP_OUT FAIL para PID %d", pcb->pid);
                }
            }

            // 3) Pasar a SUSP.READY (tiene prioridad sobre NEW)
            pthread_mutex_lock(&ctx->susp_ready_mutex);
            list_add(ctx->suspended_ready_queue, pcb);
            pthread_cond_signal(&ctx->susp_ready_cond);
            pthread_mutex_unlock(&ctx->susp_ready_mutex);

            log_info(ctx->logger,
                     "## (%d) - Pasa a SUSP. READY", pcb->pid);
            // Nota: no incrementamos i porque ya removimos este elemento
        }
        pthread_mutex_unlock(&ctx->blocked_mutex);
    }
    return NULL;
}
