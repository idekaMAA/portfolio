#ifndef K_CONTEXT_H
#define K_CONTEXT_H

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <structs/pcb/pcb.h>

typedef struct {
    t_config* config;
    t_log* logger;
    t_list* new_queue;
    t_list* suspended_ready_queue;
    t_list* ready_queue;
    t_list* cpu_sockets;
    t_list* dispatch_queue;

    pthread_mutex_t new_mutex;
    pthread_cond_t new_cond;
    pthread_mutex_t susp_ready_mutex;
    pthread_cond_t susp_ready_cond;
    pthread_mutex_t ready_mutex;
    pthread_cond_t ready_cond;
    pthread_cond_t memory_freed_cond;
    pthread_mutex_t cpu_sockets_mutex;
    pthread_mutex_t dispatch_mutex;
    pthread_cond_t dispatch_cond;
    pthread_mutex_t memory_freed_mutex;

    sem_t            dispatch_ready;
    sem_t            io_ready;
    sem_t            interrupt_ready;

    pthread_mutex_t running_mutex;
    t_pcb*          current_running;
    int             current_cpu_fd;

    t_list*         interrupt_sockets;
    pthread_mutex_t interrupt_mutex;

    t_dictionary* io_devices;
    pthread_mutex_t io_mutex;

    t_list* blocked_queue;
    pthread_mutex_t blocked_mutex;

    int suspension_time;

    char* alg_long;
    char* alg_short;
    double alpha;
    int initial_estimate;
} kernel_context_t;

#endif // K_CONTEXT_H