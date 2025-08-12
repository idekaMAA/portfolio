#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "pagination.h"

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

typedef struct{
    char* instruction;
    char** arguments;
    int arg_count;
} t_instruction; 

typedef struct {
    int pid; // it´s PC
    t_list* instructions; //index instructions
    int next_instruction_index;  //fucking index
    paging_scheme_t* page_table;
} t_pcb;

void load_instructions(); //load to memory
void load_process(int pid);
t_list* load_instructions_file(int pid);
t_pcb* get_pcb_by_pid(int pid);//
void send_instruction(t_pcb* pcb, int client_socket, t_log* logger);
void destroy_instruction(t_instruction* inst);
void destroy_pcb(t_pcb* pcb);
void destroy_all_processes(void);
void destroy_process_by_pid(int pid);

#endif //INSTRUCTIONS_H
