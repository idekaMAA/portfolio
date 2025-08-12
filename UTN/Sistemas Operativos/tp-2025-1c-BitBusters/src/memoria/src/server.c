#include "server.h"
#include "instructions.h"
#include <connection/connection.h>
#include <config/config.h>
#include <commons/config.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "mock-module.h"
#include "pagination.h"

void setup_signal_handler(void) {
    signal(SIGINT, handle_signal);
}

void start_memory_server(void) {
    t_config* config = load_config("../config/memoria.config");
    if(config == NULL){
        log_error(logger, "Couldn't load config");
        log_destroy(logger);
        exit(EXIT_FAILURE);
    }

    const char* PORT = search_port(config, "PUERTO_ESCUCHA");
    if (PORT == NULL){
        log_error(logger, "PUERTO_ESCUCHA hadn't been found in config");
        config_destroy(config);
        log_destroy(logger);
        exit(EXIT_FAILURE);
    }

    server_socket = create_server(PORT);
    if (server_socket == -1) {
        log_error(logger, "The Memory server could not be started");
        log_destroy(logger);
        exit(EXIT_FAILURE);
    }

    /*t_memory_mock* mock = create_mock_memory(1024, 64); // Cambiado de int a t_memory_mock*
    if (!mock) {
        log_error(logger, "No se pudo inicializar el mock de memoria");
        exit(EXIT_FAILURE);
    }*/

    log_info(logger, "Memory server started on port %s", PORT);
    while(true){
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int* client_socket = malloc(sizeof(int));
        if(!client_socket){
            log_error(logger, "Error creating client socket");
            continue;
        }

        *client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if(*client_socket == -1){
            log_error(logger, "Error accepting client connection");
            free(client_socket);
            continue;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) {
            log_error(logger, "Could not create thread for client");
            close(*client_socket);
            free(client_socket);
            continue;
        }
        pthread_detach(thread_id);
    }
}

void* handle_client(void* client_socket_ptr) {
    int client_socket = *(int*)client_socket_ptr;
    free(client_socket_ptr);
    // t_memory_mock* mock = create_mock_memory(1024, 64); 
    log_info(logger, "New client connected, socket: %d", client_socket);

    // Instrucciones de prueba para simular el fetch desde memoria
    char* instrucciones[] = { "NO_OP", "I/O", "EXIT" }; //ESTO SE USO DE PRUEBA
    int cantidad_instrucciones = sizeof(instrucciones) / sizeof(instrucciones[0]); //ESTO SE USO DE PRUEBA

    while(1){
        int pc;
        int bytes_received = recv_all(client_socket, &pc, sizeof(int));

    if (bytes_received != sizeof(int)) {
        log_error(logger, "Failed to receive full PC");
        close(client_socket);
        return NULL; // o terminar el hilo
    }

    //paging
    t_pcb* pcb = get_pcb_by_pid(pc);
    if(pcb && pcb->page_table){
        //translate addr
        uint32_t virtual_addr = pc;
        void* physical_addr = translate_address(pcb->page_table, virtual_addr);
        if(!physical_addr){
            log_error(logger, "Error: Fallo de página detectado - Proceso %d - Contador de Programa: %d", pcb->pid, pc);
            char* error_response = "FALLO_PAGINA";
            send(client_socket, error_response, strlen(error_response) + 1, 0);
            continue;
        }

        //traduction success
        log_info(logger,"Traducción correcta - PID %d: virtual=%u -> physical=%p", pcb->pid, virtual_addr, physical_addr);
    }
    //char* response = "Message received by Memoria";
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
                log_info(logger, "Client disconnected, socket: %d", client_socket);
            } else {
                log_error(logger, "Error receiving PC from client on socket %d", client_socket);
            }
            break;
        }

        log_info(logger, "PC recibido: %d", pc);

        char* instruccion;
        if (pc >= 0 && pc < cantidad_instrucciones) {
            instruccion = instrucciones[pc];
        } else {
            instruccion = "INVALID";  // Podés adaptar esto según tu protocolo
        }

        if (send(client_socket, instruccion, strlen(instruccion) + 1, 0) == -1) {
            log_error(logger, "Error sending instruction to client on socket %d", client_socket);
            break;
        }

        log_info(logger, "Instrucción enviada: %s", instruccion);
    }

    close(client_socket);
    log_info(logger, "Socket %d closed", client_socket);
    return NULL;
}

void handle_signal(int signal) {
    log_info(logger, "Terminating Memory server...");
    close(server_socket);
    log_destroy(logger);
    exit(EXIT_SUCCESS);
}



int recv_all(int socket, void* buffer, size_t length) {     // REVISAR
    size_t total_received = 0;
    char* buf = (char*) buffer;
    while (total_received < length) {
        int bytes = recv(socket, buf + total_received, length - total_received, 0);
        if (bytes <= 0) {
            // Error o conexión cerrada
            return bytes;
        }
        total_received += bytes;
    }
    return total_received;
}
