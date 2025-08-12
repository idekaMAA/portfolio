#include "pcb.h"

void free_state_metric(void* element) {
    t_state_metric* m = (t_state_metric*) element;
    free(m->state);
    free(m);
}

void free_time_metric(void* element) {
    t_time_metric* m = (t_time_metric*) element;
    free(m->state);
    free(m);
}

t_pcb* create_pcb(int pid, int size) {
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->pid = pid;
    pcb->pc = 0;
    pcb->process_size = size;
    pcb->state_metrics = list_create();
    pcb->time_metrics = list_create();
    return pcb;
}

int generate_pid() {
    static int pid_counter = 1;
    static pthread_mutex_t pid_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&pid_mutex);
    int pid = pid_counter++;
    pthread_mutex_unlock(&pid_mutex);

    return pid;
}

void destroy_pcb(t_pcb* pcb) {
    list_destroy_and_destroy_elements(pcb->state_metrics, free_state_metric);
    list_destroy_and_destroy_elements(pcb->time_metrics, free_time_metric);
    free(pcb);
}

void add_state_metric(t_pcb* pcb, const char* state) {
    for (int i = 0; i < list_size(pcb->state_metrics); i++) {
        t_state_metric* metric = list_get(pcb->state_metrics, i);
        if (strcmp(metric->state, state) == 0) {
            metric->count++;
            return;
        }
    }
    // Si no existe, crearla nueva
    t_state_metric* metric = malloc(sizeof(t_state_metric));
    metric->state = strdup(state);
    metric->count = 1;
    list_add(pcb->state_metrics, metric);
}

void add_time_metric(t_pcb* pcb, const char* state, double time_seconds) {
    for (int i = 0; i < list_size(pcb->time_metrics); i++) {
        t_time_metric* metric = list_get(pcb->time_metrics, i);
        if (strcmp(metric->state, state) == 0) {
            metric->time_seconds += time_seconds;
            return;
        }
    }
    // Si no existe, crearla nueva
    t_time_metric* metric = malloc(sizeof(t_time_metric));
    metric->state = strdup(state);
    metric->time_seconds = time_seconds;
    list_add(pcb->time_metrics, metric);
}

