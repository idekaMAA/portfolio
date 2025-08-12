#include <connection/connection.h>
#include <config/config.h>
#include <commons/log.h>
#include <commons/config.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include "server.h"


t_log* logger;
int server_socket;

void handle_signal(int signal);
void* handle_client(void* client_socket_ptr);

int main(int argc, char* argv[]) {
    // Create a logger to record events
    logger = log_create("memory.log", "MEMORY", true, LOG_LEVEL_INFO);
    if (logger == NULL){
        fprintf(stderr, "Error, coudln't create logger\n");
        exit(1);
    }

    log_info(logger, "Logger created successfully");

    setup_signal_handler();
    //load_instructions();      // Inicializa lista global pcb_list
    //load_process(1);          // Carga las instrucciones del proceso con pid=1 (archivo scripts/1.txt)

    start_memory_server();

    log_destroy(logger);
    return 0;
}
