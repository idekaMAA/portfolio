#ifndef RUN_CPU_H
#define RUN_CPU_H

#include <commons/log.h>
#include <commons/config.h>
#include <config/config.h>
#include <syscalls/io_syscall.h>
#include "context/k_message.h"
#include "instruction_cycle.h"
#include <syscalls/init_proc.h>


void RUN_CPU(char* cpu_id);
t_config* get_config(char* cpu_id);
int connect_dispatch(t_config* config);
int connect_interrupt(t_config* config);
int connect_memory(t_config* config);
void send_cpu_id(int socket, char* cpu_id);
void dispatcher(char* cpu_id);

#endif

