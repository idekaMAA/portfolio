#ifndef PCB_H
#define PCB_H

#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <context/k_message.h>
#include <sys/time.h>

// Métrica: cantidad de veces que pasó por un estado
typedef struct {
    char* state;     // Ej: "READY", "EXEC"
    int count;
} t_state_metric;

// Métrica: tiempo total en un estado
typedef struct {
    char* state;     // Ej: "READY", "BLOCKED"
    double time_seconds;
} t_time_metric;

// PCB del proceso
typedef struct {
    int    pid;
    int    pc;
    int    process_size;      // memoria solicitada
    int    arrival_time;      // timestamp de llegada
    int    estimated_burst;   // Est(n)
    int    remaining_time;    // ráfaga restante

    t_list* state_metrics;    // para logging
    t_list* time_metrics;     // para logging

    struct timeval blocked_time;

} t_pcb;

// Funciones públicas
t_pcb* create_pcb(int pid, int size);
void destroy_pcb(t_pcb* pcb);
void add_state_metric(t_pcb* pcb, const char* state);
void add_time_metric(t_pcb* pcb, const char* state, double time_seconds);
int generate_pid();
#endif 