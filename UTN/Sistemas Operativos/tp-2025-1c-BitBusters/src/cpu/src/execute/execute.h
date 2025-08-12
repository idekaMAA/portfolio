#ifndef EXECUTE_H
#define EXECUTE_H
#include "context/k_message.h"
#include <stdbool.h>        
#include <commons/log.h>  
#include "decode/opcode.h"
#include <syscalls/init_proc.h>
#include <connection/connection.h>

extern int pc;

void execute_noop(void);
void execute_write(struct opcode op, int memory_socket);
void execute_read(struct opcode op, int memory_socket);
void execute_goto(struct opcode op, k_message* k_message);
void execute_io(struct opcode op, int pid, int dispatch_socket);
void execute_init_proc(struct opcode op, int dispatch_socket);
void execute_dump_memory(void);

#endif
