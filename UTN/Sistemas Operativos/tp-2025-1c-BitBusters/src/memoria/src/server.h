#ifndef SERVER_H
#define SERVER_H

#include <commons/log.h>


extern t_log* logger;
extern int server_socket;

void handle_signal(int sig);
void start_memory_server(void);
void* handle_client(void* client_socket_ptr);
void setup_signal_handler(void);
int recv_all(int socket, void* buffer, size_t length);

#endif //SERVER_H
