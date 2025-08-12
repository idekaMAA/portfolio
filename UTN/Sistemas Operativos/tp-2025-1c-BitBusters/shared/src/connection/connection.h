#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

typedef enum {
    MESSAGE,
    PACKAGE,
    WRITE_OP,
    READ_OP
} t_operation_code;

typedef struct {
    int size;
    void* stream;
} t_buffer;

typedef struct {
    t_operation_code operation_code;
    t_buffer* buffer;
} t_package;

void* serialize_packet(t_package* packet, int total_bytes);
int create_connection(const char* ip, const char* port);
int create_server(const char* port);
void send_message(char* message, int client_socket);
void create_buffer(t_package* packet);
t_package* create_packet(void);
void add_to_packet(t_package* packet, void* value, int size);
void send_packet(t_package* packet, int client_socket);
void delete_packet(t_package* packet);
void free_connection(int client_socket);

#endif