//#include <shared/hello.h>
#include <connection/connection.h>
#include <commons/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <config/config.h>
#include <syscalls/io_syscall.h>

t_log* logger;

int main(int argc, char* argv[]) {
    logger = log_create("io.log", "IO", true, LOG_LEVEL_INFO);

    if (argc < 2) {
        log_error(logger, "Uso esperado: %s <nombre_interfaz_io>", argv[0]);
        log_destroy(logger);
        return EXIT_FAILURE;
    }

    char* io_interface_name = argv[1];
    log_info(logger, "Nombre interfaz IO: %s", io_interface_name);

    t_config* config = load_config("../config/io.config");
    const char* KERNEL_IP = search_ip(config,"KERNEL_IP");
    const char* KERNEL_PORT = search_port(config, "KERNEL_PORT");

    int kernel_socket = create_connection(KERNEL_IP, KERNEL_PORT);
    if (kernel_socket == -1) {
        log_error(logger, "No se pudo conectar al servidor kernel");
        log_destroy(logger);
        return EXIT_FAILURE;
    }
    log_info(logger, "Conectado con kernel");

    // send_message(io_interface_name, kernel_socket); 
    send(kernel_socket, io_interface_name, strlen(io_interface_name) + 1, 0);
    log_info(logger, "Dispositivo IO '%s' conectado al Kernel", io_interface_name);

    while (1) {
        char buffer[1024] = {0};
        int bytes = recv(kernel_socket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            log_error(logger, "Kernel desconectado");
            break;
        }
    
        log_info(logger, "Bytes recibidos del Kernel: %d", bytes);
        t_syscall_io* syscall = deserialize_syscall_io(buffer, bytes);
        if (!syscall || !syscall->device) {
            log_error(logger, "Error al deserializar syscall_io");
            continue;
        }
        log_info(logger, "## PID: %d - Inicio de IO - Tiempo: %d", syscall->pid, syscall->time);
    
        // Simular trabajo
        usleep(syscall->time * 1000);
        log_info(logger, "## PID: %d - Fin de IO", syscall->pid);
        char fin_io_msg[64];
        snprintf(fin_io_msg, sizeof(fin_io_msg), "FIN_IO %d", syscall->pid);
        send(kernel_socket, fin_io_msg, strlen(fin_io_msg) + 1, 0);
    
        syscall_io_destroy(syscall);
    }

    free_connection(kernel_socket);

    return EXIT_SUCCESS;
}