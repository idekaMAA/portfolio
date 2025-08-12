#ifndef FOLLOW_INSTRUCTIONS_H
#define FOLLOW_INSTRUCTIONS_H

#include <commons/log.h>
#include <structs/memory-cpu/op_code.h>
#include <sys/types.h>  // ssize_t

void handle_execute_instruction(int client_socket);

// Variables externas
extern t_log* logger;

#endif // FOLLOW_INSTRUCTIONS_H
