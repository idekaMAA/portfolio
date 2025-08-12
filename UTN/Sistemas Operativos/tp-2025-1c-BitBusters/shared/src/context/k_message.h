#ifndef K_MESSAGE_H
#define K_MESSAGE_H
#include <stdint.h>

typedef struct {
    uint32_t pid;
    int pc;
    int process_size; 
} k_message;

k_message* context_create(int pid, int pc);
void* context_serialize(k_message* context, int* size);
k_message* context_deserialize(void* buffer);
void context_destroy(k_message* context);

#endif