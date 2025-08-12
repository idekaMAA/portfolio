#include "short_term_scheduler.h"

#define INTERRUPT_CODE 5

t_pcb* select_from_queue(t_list* queue, const char* alg) {
    if (strcmp(alg, "FIFO") == 0) {
        return list_remove(queue, 0);
    }
    else if (strcmp(alg, "SJF") == 0) {
        int idx = find_index_min(queue, cmp_estburst);
        return list_remove(queue, idx);
    }
    else { // SRT
        int idx = find_index_min(queue, cmp_remaining);
        return list_remove(queue, idx);
    }
}


void* short_term_scheduler(void* context) {
    kernel_context_t* ctx = (kernel_context_t*)context;
    log_info(ctx->logger, "Planificador de corto plazo iniciado.");

    while (true) {
        t_pcb* next = NULL;

        // 1) Prioridad a SUSPENDED_READY
        pthread_mutex_lock(&ctx->susp_ready_mutex);
        if (!list_is_empty(ctx->suspended_ready_queue)) {
            // Extraemos según algoritmo
            if (strcmp(ctx->alg_short, "FIFO") == 0) {
                next = list_remove(ctx->suspended_ready_queue, 0);
            } else if (strcmp(ctx->alg_short, "SJF") == 0) {
                int idx = find_index_min(ctx->suspended_ready_queue, cmp_estburst);
                next = list_remove(ctx->suspended_ready_queue, idx);
            } else { // SRT
                int idx = find_index_min(ctx->suspended_ready_queue, cmp_remaining);
                next = list_remove(ctx->suspended_ready_queue, idx);
            }
        }
        pthread_mutex_unlock(&ctx->susp_ready_mutex);

        // 2) Si no había en SUSPENDED_READY, tomo de READY
        if (!next) {
            pthread_mutex_lock(&ctx->ready_mutex);

            // PREEMPCIÓN SRT: antes de sacar, miro al recién llegado
            if (strcmp(ctx->alg_short, "SRT") == 0 && 
                !list_is_empty(ctx->ready_queue)) {
                // Índice del proceso con menor remaining_time
                int idx_new = find_index_min(ctx->ready_queue, cmp_remaining);
                t_pcb* newcomer = list_get(ctx->ready_queue, idx_new);

                pthread_mutex_lock(&ctx->running_mutex);
                t_pcb* running = ctx->current_running;
                int    cpu_fd  = ctx->current_cpu_fd;
                pthread_mutex_unlock(&ctx->running_mutex);

                if (running && newcomer->remaining_time < running->remaining_time) {
                    send_interrupt_to(cpu_fd);
                    log_info(ctx->logger,
                             "## (%d) – Desalojado por SRT", running->pid);
                }
            }

            // Espero si no hay ninguno listo
            while (list_is_empty(ctx->ready_queue)) {
                pthread_cond_wait(&ctx->ready_cond, &ctx->ready_mutex);
            }

            // Extraigo el siguiente según algoritmo
            if (strcmp(ctx->alg_short, "FIFO") == 0) {
                next = list_remove(ctx->ready_queue, 0);
            } else if (strcmp(ctx->alg_short, "SJF") == 0) {
                int idx = find_index_min(ctx->ready_queue, cmp_estburst);
                next = list_remove(ctx->ready_queue, idx);
            } else { // SRT
                int idx = find_index_min(ctx->ready_queue, cmp_remaining);
                next = list_remove(ctx->ready_queue, idx);
            }

            pthread_mutex_unlock(&ctx->ready_mutex);
        }

        // 3) Encolar para dispatch
        log_info(ctx->logger,
                 "## (%d) Pasa del estado READY al estado EXEC", next->pid);

        pthread_mutex_lock(&ctx->dispatch_mutex);
        list_add(ctx->dispatch_queue, next);
        pthread_cond_signal(&ctx->dispatch_cond);
        pthread_mutex_unlock(&ctx->dispatch_mutex);
    }

    return NULL;
}


// Comparison function for SJF algorithm
int cmp_estburst(void* a, void* b) {
    t_pcb* pcb_a = (t_pcb*)a;
    t_pcb* pcb_b = (t_pcb*)b;
    return pcb_a->estimated_burst - pcb_b->estimated_burst;
}

// Comparison function for SRT algorithm
int cmp_remaining(void* a, void* b) {
    t_pcb* pcb_a = (t_pcb*)a;
    t_pcb* pcb_b = (t_pcb*)b;
    return pcb_a->remaining_time - pcb_b->remaining_time;
}

int find_index_short(t_list* list, bool (*condition)(void*, void*), void* context) {
    for (int i = 0; i < list_size(list); i++) {
        if (condition(list_get(list, i), context)) {
            return i;
        }
    }
    return -1; // No encontrado
}

bool match_pcb_short(void* element, void* context) {
    return element == context;
}

// Devuelve el índice del PCB con cmp(a,b) < 0 siendo “a” más prioritario que “b”
int find_index_min(t_list* list, 
    int (*cmp)(void* a, void* b)) {
    int best = 0;
    for (int i = 1; i < list_size(list); i++) {
        if (cmp(list_get(list, i), list_get(list, best)) < 0) {
            best = i;
        }
    }
    return best;
}

void send_interrupt_to(int cpu_fd) {
    uint32_t code = htonl(INTERRUPT_CODE);
    send(cpu_fd, &code, sizeof(code), 0);
}

void send_interrupt_all(kernel_context_t* ctx) {
    pthread_mutex_lock(&ctx->interrupt_mutex);
    for (int i = 0; i < list_size(ctx->interrupt_sockets); i++) {
        int fd = (int)(intptr_t)list_get(ctx->interrupt_sockets, i);
        send_interrupt_to(fd);
    }
    pthread_mutex_unlock(&ctx->interrupt_mutex);
}

