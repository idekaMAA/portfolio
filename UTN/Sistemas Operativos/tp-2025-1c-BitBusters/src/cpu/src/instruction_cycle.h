#ifndef INSTRUCTION_CYCLE_H
#define INSTRUCTION_CYCLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "execute/execute.h"
#include <sys/socket.h>
#include <commons/log.h>
#include "context/k_message.h"
#include "decode/opcode.h"

char *fetch(int memory_socket, int pc);
struct opcode decode(char *instructionStr);
void free_opcode(struct opcode *op);
void execute(struct opcode op, int memory_socket, int kernel_dispatch_socket, int kernel_interrupt_socket, k_message *k_message);
bool instruction_cycle(k_message *k_message, int memory_socket, int kernel_dispatch_socket, int kernel_interrupt_socket);
void execute_exit(int kernel_dispatch_socket, k_message *k_message);
bool checkInterrupt(int interruptIdentificador, int kernel_interrupt_socket, int pidInterrup, int pcInterrup);

#endif
