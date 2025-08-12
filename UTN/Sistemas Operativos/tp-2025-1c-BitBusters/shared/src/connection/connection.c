#include "connection.h"

void* serialize_packet(t_package* packet, int total_bytes) {
    void* serialized_data = malloc(total_bytes);
    int offset = 0;

    memcpy(serialized_data + offset, &(packet->operation_code), sizeof(int));
    offset += sizeof(int);
    memcpy(serialized_data + offset, &(packet->buffer->size), sizeof(int));
    offset += sizeof(int);
    memcpy(serialized_data + offset, packet->buffer->stream, packet->buffer->size);

    return serialized_data;
}

int create_connection(const char* ip, const char* port) {
    int client_socket;
    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 

    if (getaddrinfo(ip, port, &hints, &server_info) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    for (p = server_info; p != NULL; p = p->ai_next) {
        client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (client_socket == -1) {
            perror("socket");
            continue;
        }

        if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(client_socket);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect to server\n");
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);
    return client_socket;
}

int create_server(const char* port) {
    int server_socket;
    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;     

    if (getaddrinfo(NULL, port, &hints, &server_info) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    for (p = server_info; p != NULL; p = p->ai_next) {
        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket == -1) {
            perror("socket");
            continue;
        }

        int optval = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(server_socket);
            perror("bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to bind socket\n");
        freeaddrinfo(server_info);
        return -1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        perror("listen");
        close(server_socket);
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);
    return server_socket;
}

void send_message(char* message, int client_socket) {
    t_package* packet = malloc(sizeof(t_package));
    packet->operation_code = MESSAGE;
    packet->buffer = malloc(sizeof(t_buffer));
    packet->buffer->size = strlen(message) + 1;
    packet->buffer->stream = malloc(packet->buffer->size);
    memcpy(packet->buffer->stream, message, packet->buffer->size);

    int total_bytes = packet->buffer->size + 2 * sizeof(int);
    void* serialized_data = serialize_packet(packet, total_bytes);

    send(client_socket, serialized_data, total_bytes, 0);

    free(serialized_data);
    delete_packet(packet);
}

void create_buffer(t_package* packet) {
    packet->buffer = malloc(sizeof(t_buffer));
    packet->buffer->size = 0;
    packet->buffer->stream = NULL;
}

t_package* create_packet(void) {
    t_package* packet = malloc(sizeof(t_package));
    packet->operation_code = PACKAGE;
    create_buffer(packet);
    return packet;
}

void add_to_packet(t_package* packet, void* value, int size) {
    packet->buffer->stream = realloc(packet->buffer->stream, packet->buffer->size + size + sizeof(int));

    memcpy(packet->buffer->stream + packet->buffer->size, &size, sizeof(int));
    memcpy(packet->buffer->stream + packet->buffer->size + sizeof(int), value, size);

    packet->buffer->size += size + sizeof(int);
}

void send_packet(t_package* packet, int client_socket) {
    int total_bytes = packet->buffer->size + 2 * sizeof(int);
    void* serialized_data = serialize_packet(packet, total_bytes);

    send(client_socket, serialized_data, total_bytes, 0);

    free(serialized_data);
}

void delete_packet(t_package* packet) {
    free(packet->buffer->stream);
    free(packet->buffer);
    free(packet);
}

void free_connection(int client_socket) {
    close(client_socket);
}