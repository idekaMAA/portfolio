#include <stdio.h>
#include <connection/connection.h> 
#include "instructions.h"
#include <commons/config.h>
#include <commons/log.h>


t_list* pbc_list;
extern t_log* logger;


t_list* pcb_list; //global process list
extern t_log* logger;

void load_instructions(){
    pcb_list = list_create();
}
static bool match_pcb(void* pcb){
    return true;
}

t_list* load_instructions_file(int pid){
    char path[64];
    sprintf(path, "scripts/%d.txt", pid);

    FILE* file = fopen(path, "r");
    if (!file) return NULL;

    t_list* instruction_list = list_create();
    char line[256];

    while (fgets(line, sizeof(line), file)){
        line[strcspn(line,"\n")] = 0; //delete the final when it comes

        t_instruction* inst = malloc(sizeof(t_instruction));
        inst->instruction = strdup(strtok(line, " "));

        inst->arg_count = 0;
        inst->arguments = malloc(sizeof(char*) *10);  //10 is the argu. max

        char* token;
        while((token = strtok(NULL, " ")) != NULL){
            inst->arguments[inst->arg_count++] = strdup(token);
        }

        list_add(instruction_list, inst);
    }

    fclose(file);
    return instruction_list;
}

void load_process(int pid){
    t_list* instructions = load_instructions_file(pid);
    if (!instructions) return;

    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->pid = pid;
    pcb->instructions = instructions;
    pcb->next_instruction_index = 0;

    //load page table
    //get from config
    t_config* config = config_create("../config/memoria.config");
    if (config == NULL){
        log_error(logger, "No se pudo cargar la configuración");
        return;
    }

    int global_paging_levels = config_get_int_value(config, "CANTIDAD_NIVELES");
    int table_inputs = config_get_int_value(config, "ENTRADAS_POR_TABLA");
    int page_size = config_get_int_value(config, "TAM_PAGINA");

    paging_scheme_t* scheme = int_paging_scheme(global_paging_levels, table_inputs, page_size);
    pcb -> page_table = scheme;

    config_destroy(config);
    list_add(pcb_list, pcb);
}

t_pcb* get_pcb_by_pid(int pid){
    for (int i = 0; i < list_size(pcb_list); i++){
        t_pcb* pcb = list_get(pcb_list, i);
        if (pcb->pid == pid) return pcb;
    }

    return NULL;
}


void send_instruction(t_pcb* pcb, int client_socket, t_log* logger){
    if (pcb == NULL || pcb->instructions == NULL){
        log_error(logger, "PID: %d - Error: no instruction found for PC %d", pcb ? pcb->pid : -1, pcb ? pcb->next_instruction_index : -1);
        return;
    }

    if(pcb->next_instruction_index >= list_size(pcb->instructions)){
        log_warning(logger,"## PID: %d - No more instructions to send", pcb->pid);
        return;
    }

    //get current instruction
    t_instruction* inst = list_get(pcb->instructions, pcb->next_instruction_index);

    //full instruction string
    char buffer[256] = {0};
    strcat(buffer, inst->instruction);

    for(int i = 0; i < inst->arg_count; i++){
        strcat(buffer, " ");
        strcat(buffer, inst->arguments[i]);
    }

    log_info(logger, "## PID: %d - get instruction: %d - instruction: %s", pcb->pid, pcb->next_instruction_index, buffer);

    //send to cpu
    send_message(buffer, client_socket);

    pcb->next_instruction_index++; //go pc go
}

void destroy_instruction(t_instruction* inst){
    if (!inst) return;

    free(inst->instruction);

    for(int i = 0; i < inst->arg_count; i++){
        free(inst->arguments[i]);
    }

    free(inst->arguments);
    free(inst);
}

void destroy_pcb(t_pcb* pcb){
    if(!pcb) return;

    list_destroy_and_destroy_elements(pcb->instructions, (void*) destroy_instruction);
    free(pcb);
}

void destroy_all_processes(void){
    list_remove_and_destroy_by_condition(pcb_list,match_pcb, (void*) destroy_pcb);
}

void destroy_process_by_pid(int pid){

    bool match_pcb(void* element){
        t_pcb* pcb = (t_pcb*) element;
        return pcb->pid == pid;
    }

    t_pcb* found_pcb = list_find(pcb_list, match_pcb);
    if(!found_pcb){
        log_error(logger, "Process PID %d not found for destruction", pid);
        return;
    }
    list_remove_and_destroy_by_condition(pcb_list, match_pcb, (void*) destroy_pcb);
    log_info(logger, "Process PID %d destroeyed successfully", pid);
}

