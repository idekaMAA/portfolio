#include "scheduler/long_term_scheduler.h"
#include "connections/memory_connection.h"
#include "context/k_context.h"
#include "structs/pcb/pcb.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define OPCODE_INIT 1      // según tu protocolo con Memoria
#define RESP_OK     0      // Memoria responde 0 = OK, 1 = FAIL

// Extrae de NEW según FIFO o Proceso Más Chico Primero (PMCP)
t_pcb* select_from_new(kernel_context_t* ctx) {
    if (strcmp(ctx->alg_long, "FIFO") == 0) {
        return list_remove(ctx->new_queue, 0);
    } else {
        int idx_min = 0;
        t_pcb* min_pcb = list_get(ctx->new_queue, 0);
        for (int i = 1; i < list_size(ctx->new_queue); i++) {
            t_pcb* pcb = list_get(ctx->new_queue, i);
            if (pcb->process_size < min_pcb->process_size) {
                min_pcb = pcb;
                idx_min = i;
            }
        }
        return list_remove(ctx->new_queue, idx_min);
    }
}

void* long_term_scheduler(void* arg) {
    kernel_context_t* ctx = (kernel_context_t*)arg;
    log_info(ctx->logger, "Planificador de largo plazo iniciado.");

    while (true) {
        // 1) Espero a que NEW tenga algo **y** SUSP. READY esté vacío
        pthread_mutex_lock(&ctx->new_mutex);
        while ( list_is_empty(ctx->new_queue)
             || !list_is_empty(ctx->suspended_ready_queue) ) {
            pthread_cond_wait(&ctx->new_cond, &ctx->new_mutex);
        }

        // 2) Seleccionar y extraer PCB de NEW
        t_pcb* pcb = select_from_new(ctx);
        pthread_mutex_unlock(&ctx->new_mutex);

        // 3) Conectar con Memoria
        const char* ip   = search_ip(ctx->config,   "IP_MEMORY");
        const char* port = search_port(ctx->config, "PORT_MEMORY");
        int mem_fd = create_connection(ip, port);
        if (mem_fd < 0) {
            log_error(ctx->logger, "LP: no pude conectar a Memoria");
            // reintento tras liberación de memoria
            pthread_cond_wait(&ctx->memory_freed_cond, &ctx->new_mutex);
            // reencolar PCB
            pthread_mutex_lock(&ctx->new_mutex);
            list_add(ctx->new_queue, pcb);
            pthread_cond_signal(&ctx->new_cond);
            pthread_mutex_unlock(&ctx->new_mutex);
            continue;
        }

        // 4) Enviar INIT (opcode + PID + tamaño)
        uint8_t  opcode = OPCODE_INIT;
        uint32_t pid_n  = htonl(pcb->pid);
        uint32_t size_n = htonl(pcb->process_size);
        send(mem_fd, &opcode, sizeof(opcode), 0);
        send(mem_fd, &pid_n,   sizeof(pid_n),   0);
        send(mem_fd, &size_n,  sizeof(size_n),  0);

        // 5) Recibir respuesta de Memoria
        uint8_t resp;
        if (recv(mem_fd, &resp, sizeof(resp), MSG_WAITALL) != sizeof(resp)) {
            log_error(ctx->logger, "LP: falla recv respuesta INIT");
            resp = 1;  // fuerza FAIL
        }
        close(mem_fd);

        if (resp == RESP_OK) {
            // 6a) OK → paso a READY
            log_info(ctx->logger, "## (%d) Pasa del estado NEW a READY", pcb->pid);
            pthread_mutex_lock(&ctx->ready_mutex);
            list_add(ctx->ready_queue, pcb);
            pthread_cond_signal(&ctx->ready_cond);
            pthread_mutex_unlock(&ctx->ready_mutex);
        } else {
            // 6b) FAIL → reencolo en NEW tras señal de memoria liberada
            log_warning(ctx->logger,
                        "LP: espacio insuficiente para %d, espero liberación",
                        pcb->pid);
            pthread_mutex_lock(&ctx->memory_freed_mutex);
            pthread_cond_wait(&ctx->memory_freed_cond, &ctx->memory_freed_mutex);
            pthread_mutex_unlock(&ctx->memory_freed_mutex);

            pthread_mutex_lock(&ctx->new_mutex);
            list_add(ctx->new_queue, pcb);
            pthread_cond_signal(&ctx->new_cond);
            pthread_mutex_unlock(&ctx->new_mutex);
        }
    }
    return NULL;
}

int find_index(t_list* list, bool (*condition)(void*, void*), void* context) {
    for (int i = 0; i < list_size(list); i++) {
        if (condition(list_get(list, i), context)) {
            return i;
        }
    }
    return -1; // No encontrado
}

bool match_pcb(void* element, void* context) {
    t_pcb* pcb = (t_pcb*)element;
    t_pcb* target = (t_pcb*)context;
    return pcb == target;
}

int cmp_memsize(void* a, void* b) {
    t_pcb* pcb_a = (t_pcb*)a;
    t_pcb* pcb_b = (t_pcb*)b;
    return pcb_a->process_size - pcb_b->process_size;
}

void enqueue_ready(kernel_context_t* ctx, t_pcb* p) {
    pthread_mutex_lock(&ctx->ready_mutex);
    list_add(ctx->ready_queue, p);
    pthread_cond_signal(&ctx->ready_cond);
    pthread_mutex_unlock(&ctx->ready_mutex);
}

void signal_memory_freed(kernel_context_t* ctx) {
    pthread_mutex_lock(&ctx->memory_freed_mutex);
    pthread_cond_signal(&ctx->memory_freed_cond);
    pthread_mutex_unlock(&ctx->memory_freed_mutex);
}

