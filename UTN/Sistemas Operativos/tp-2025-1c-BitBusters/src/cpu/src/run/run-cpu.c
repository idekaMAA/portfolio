#include "run-cpu.h"
#include "connections/connection_cpu.h"

t_log* logger;

void RUN_CPU(char* cpu_id) 
{

    // int socket_interrupt = connect_interrupt(config);
    // int socket_memory = connect_memory(config);
    // send_cpu_id(socket_interrupt, cpu_id);
    // send_cpu_id(socket_memory, cpu_id);
    dispatcher(cpu_id);

    log_destroy(logger);
}

t_config* get_config(char* cpu_id){
    char log_filename[64];
    snprintf(log_filename, sizeof(log_filename), "cpu-%s.log", cpu_id);
    logger = log_create(log_filename, "CPU", true, LOG_LEVEL_INFO);

    log_info(logger, "Iniciando instancia CPU con ID: %s", cpu_id);
    t_config* config = load_config("../config/cpu.config");
    if (config == NULL) {
        log_error(logger, "Falla al cargar archivo de configuración. Saliendo.");
        log_destroy(logger);
        return NULL;
    }

    return config;
}

int connect_dispatch(t_config* config){
    const char* ip_kernel = search_ip(config, "IP_KERNEL");
    const char* port_kernel_dispatch = search_port(config, "PORT_KERNEL_DISPATCH");

    int socket_dispatch = connect_to_kernel_dispatch(ip_kernel, port_kernel_dispatch);
    if (socket_dispatch == -1) {
        log_error(logger, "Falla al conectarse al kernel dispatch. Saliendo.");
        log_destroy(logger);
    }
    return socket_dispatch;
}

int connect_interrupt(t_config* config){
    const char* ip_kernel = search_ip(config, "IP_KERNEL");
    const char* port_kernel_interrupt = search_port(config, "PORT_KERNEL_INTERRUPT");

    int socket_interrupt = connect_to_kernel_interrupt(ip_kernel, port_kernel_interrupt);
    if (socket_interrupt == -1) {
        log_error(logger, "Falla al conectarse al kernel interrupt. Saliendo.");
        log_destroy(logger);
    }
    return socket_interrupt;
}

int connect_memory(t_config* config){
    const char* ip_memory = search_ip(config, "IP_MEMORY");
    const char* port_memory = search_port(config, "PORT_MEMORY");

    int socket_memory = connect_to_memory(ip_memory, port_memory);
    if (socket_memory == -1) {
        log_error(logger, "Falla al conectarse a memoria. Saliendo.");
        log_destroy(logger);
    }
    return socket_memory;
}

void send_cpu_id(int socket, char* cpu_id) {
    if (send(socket, cpu_id, strlen(cpu_id) + 1, 0) == -1) {
        log_error(logger, "Falla al enviar el CPU ID al kernel.");
        return;
    }
    log_info(logger, "CPU ID enviado al kernel: %s", cpu_id);
}

void dispatcher(char* cpu_id) 
{
    t_config* config = get_config(cpu_id);
    int socket_dispatch = connect_dispatch(config);
    int socket_interrupt = connect_interrupt(config);
    int socket_memory = connect_memory(config);

    if (socket_dispatch == -1) {
        log_error(logger, "Falla al conectarse al kernel dispatch. Saliendo.");
        return;
    }

    send_cpu_id(socket_dispatch, cpu_id);

    while (1) {
        void* buffer = malloc(sizeof(int) * 2);
        int bytes = recv(socket_dispatch, buffer, sizeof(int) * 2, 0);

        if (bytes <= 0) {
            log_error(logger, "Conexión con kernel dispatch perdida o fallo al recibir datA.");
            free(buffer);
            break;
        }

        k_message* context = context_deserialize(buffer);
    

        log_info(logger, "PCB recibido: PID=%d | PC=%d", context->pid, context->pc);

        while (instruction_cycle(context, socket_memory, socket_dispatch, socket_interrupt));

        context_destroy(context);
        free(buffer);
    }

    log_info(logger, "Apagando instancia CPU.");
    log_destroy(logger);
}