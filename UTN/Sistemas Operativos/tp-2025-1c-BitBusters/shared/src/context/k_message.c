#include "k_message.h"
#include <stdlib.h>
#include <string.h>

k_message* context_create(int pid, int pc) {
    k_message* ctx = malloc(sizeof(k_message));
    ctx->pid = pid;
    ctx->pc = pc;
    return ctx;
}

void* context_serialize(k_message* context, int* size) {
    *size = sizeof(int) * 2;
    void* buffer = malloc(*size);
    memcpy(buffer, &context->pid, sizeof(int));
    memcpy(buffer + sizeof(int), &context->pc, sizeof(int));
    return buffer;
}

k_message* context_deserialize(void* buffer) {
    k_message* ctx = malloc(sizeof(k_message));
    memcpy(&ctx->pid, buffer, sizeof(int));
    memcpy(&ctx->pc, buffer + sizeof(int), sizeof(int));
    return ctx;
}

void context_destroy(k_message* context) {
    free(context);
}
