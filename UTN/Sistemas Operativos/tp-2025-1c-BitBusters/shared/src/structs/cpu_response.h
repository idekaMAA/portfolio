
#define CPU_RESPONSE_H

typedef enum {
    CPU_FINISHED,        // El proceso terminó su ejecución
    CPU_BLOCKED,         // El proceso se bloqueó (por I/O u otro evento)
    CPU_EXPIRED_QUANTUM, // Terminó su quantum (para RR o SRT)
    CPU_PREEMPTED        // Fue desalojado por otro proceso (SRT)
} cpu_response_reason_t;

typedef struct {
    int                     pid;            // PID del proceso que devolvió
    cpu_response_reason_t   reason;         // Motivo de la devolución
    int                     pc;             // Nuevo valor de PC tras ejecución/interrupción
    int                     remaining_time; // Tiempo restante de ráfaga (para SRT/SJF)
} cpu_response_t;

