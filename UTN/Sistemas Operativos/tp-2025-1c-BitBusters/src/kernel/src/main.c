#include <signal.h>
#include <servers/dispatch_listener.h>
#include <servers/interrupt_listener.h>
#include <servers/io_listener.h>
#include <connections/memory_connection.h>
#include <context/k_context.h>
#include <structs/pcb/pcb.h>
#include <scheduler/long_term_scheduler.h>
#include <scheduler/short_term_scheduler.h>
#include <scheduler/mid_term_scheduler.h>
#include <config/config.h>


t_log* logger;

void handle_signal(int signal);

int main(int argc, char* argv[]) {
    kernel_context_t kernel_ctx;

    // 1. Logger, config y señal
    kernel_ctx.logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    logger = kernel_ctx.logger;
    kernel_ctx.config = load_config("../config/kernel.config");
    signal(SIGINT, handle_signal);

    // 2. Inicializar semáforos (empiezan en 0)
    sem_init(&kernel_ctx.dispatch_ready,  0, 0);
    sem_init(&kernel_ctx.io_ready,        0, 0);
    sem_init(&kernel_ctx.interrupt_ready, 0, 0);

    // 3. Cargar algoritmos y parámetros
    kernel_ctx.alg_long         = config_get_string(kernel_ctx.config, "ALGORITMO_INGRESO_A_READY");
    kernel_ctx.alg_short        = config_get_string(kernel_ctx.config, "ALGORITMO_CORTO_PLAZO");
    kernel_ctx.alpha            = config_get_double(kernel_ctx.config, "ALFA");
    kernel_ctx.initial_estimate = config_get_int   (kernel_ctx.config, "INITIAL_ESTIMATE");
    kernel_ctx.suspension_time = config_get_int(kernel_ctx.config, "TIEMPO_SUSPENSION");

    // 4. Crear colas
    kernel_ctx.new_queue             = list_create();
    kernel_ctx.suspended_ready_queue = list_create();
    kernel_ctx.ready_queue           = list_create();
    kernel_ctx.cpu_sockets           = list_create();
    kernel_ctx.dispatch_queue        = list_create();
    kernel_ctx.interrupt_sockets     = list_create();
    kernel_ctx.io_devices            = dictionary_create();
    kernel_ctx.blocked_queue         = list_create();
    kernel_ctx.current_running       = NULL;
    kernel_ctx.current_cpu_fd        = -1;

    // 5. Inicializar mutexes y condiciones
    pthread_mutex_init(&kernel_ctx.new_mutex,           NULL);
    pthread_cond_init (&kernel_ctx.new_cond,            NULL);
    pthread_mutex_init(&kernel_ctx.susp_ready_mutex,    NULL);
    pthread_cond_init (&kernel_ctx.susp_ready_cond,     NULL);
    pthread_mutex_init(&kernel_ctx.ready_mutex,         NULL);
    pthread_cond_init (&kernel_ctx.ready_cond,          NULL);
    pthread_mutex_init(&kernel_ctx.memory_freed_mutex,  NULL);
    pthread_cond_init (&kernel_ctx.memory_freed_cond,   NULL);
    pthread_mutex_init(&kernel_ctx.cpu_sockets_mutex,   NULL);
    pthread_mutex_init(&kernel_ctx.dispatch_mutex,      NULL);
    pthread_cond_init (&kernel_ctx.dispatch_cond,       NULL);
    pthread_mutex_init(&kernel_ctx.running_mutex,       NULL);
    pthread_mutex_init(&kernel_ctx.interrupt_mutex,     NULL);
    pthread_mutex_init(&kernel_ctx.io_mutex,            NULL);


    // 6. Si se pasaron args, creo el primer proceso
    if (argc >= 3) {
        char* file_pseudocode = argv[1];
        int process_size      = atoi(argv[2]);
        log_info(logger, "Archivo pseudocódigo: %s", file_pseudocode);
        log_info(logger, "Tamaño de proceso: %d", process_size);

        t_pcb* pcb = create_pcb(generate_pid(), process_size);

        pthread_mutex_lock(&kernel_ctx.new_mutex);
        list_add   (kernel_ctx.new_queue, pcb);
        pthread_cond_signal(&kernel_ctx.new_cond);
        pthread_mutex_unlock(&kernel_ctx.new_mutex);

        log_info(logger, "## (%d) Se crea el proceso - Estado: NEW", pcb->pid);
    } else {
        log_info(logger, "Arranque sin proceso inicial, esperando INIT_PROC por dispatch");
    }

    // 7. Arrancar listeners **antes** del ENTER
    pthread_t dispatch_thread, interrupt_thread, io_thread;

    log_info(logger, "Activando Dispatch Listener…");
    pthread_create(&dispatch_thread,  NULL, dispatch_listener,  &kernel_ctx);
    pthread_detach(dispatch_thread);

    log_info(logger, "Activando IO Listener…");
    pthread_create(&io_thread,       NULL, io_listener,       &kernel_ctx);
    pthread_detach(io_thread);

    log_info(logger, "Activando Interrupt Listener…");
    pthread_create(&interrupt_thread, NULL, interrupt_listener, &kernel_ctx);
    pthread_detach(interrupt_thread);

    // 8. Esperar a que los 3 threads confirmen que ya están listening
    sem_wait(&kernel_ctx.dispatch_ready);
    sem_wait(&kernel_ctx.io_ready);
    sem_wait(&kernel_ctx.interrupt_ready);

    // Ahora sí mostramos el prompt
    printf("Presione ENTER para iniciar Planificador de Largo Plazo…\n");
    getchar();

    // 9. Arrancar planificadores
    pthread_t lt_thread,mt_thread, st_thread;

    log_info(logger, "Activando Planificador de Largo Plazo");
    pthread_create(&lt_thread, NULL, long_term_scheduler,  &kernel_ctx);
    pthread_detach(lt_thread);

    log_info(logger, "Activando Planificador de Mediano Plazo");
    pthread_create(&mt_thread, NULL, mid_term_scheduler, &kernel_ctx);
    pthread_detach(mt_thread);

    log_info(logger, "Activando Planificador de Corto Plazo");
    pthread_create(&st_thread, NULL, short_term_scheduler, &kernel_ctx);
    pthread_detach(st_thread);

    // 10. Queda a la espera de señales y eventos
    pause();
    return 0;
}

void handle_signal(int signal) {
    log_info(logger, "Kernel finalizado");
    log_destroy(logger);
    exit(EXIT_SUCCESS);
}