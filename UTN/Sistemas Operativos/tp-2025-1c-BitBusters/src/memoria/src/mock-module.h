#ifndef MOCK_MODULE_H
#define MOCK_MODULE_H

#include <stdbool.h>
#include <time.h>
#include <commons/collections/dictionary.h>
#include "pagination.h"
#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    void* base;
    size_t size;
    size_t total_size;
    paging_scheme_t* pagination;
    bool* free_slots;
    size_t total_slots;
    t_dictionary* process_table;
} t_memory_mock;

// Funciones principales
t_memory_mock* create_mock_memory(size_t total_size, size_t block_size);
void destroy_memory_mock(t_memory_mock* mock);

// Funciones de gestión de memoria
void* memory_mock_alloc(t_memory_mock* mock, int pid, uint32_t virtual_addr);
void memory_mock_free(t_memory_mock* mock, void* address);
void destroy_memory_mock(t_memory_mock* mock);
void memory_mock_dump(t_memory_mock* mock, int pid);

// Funciones auxiliares
int get_free_slots_count(bool* free_slots, int total_slots);
bool is_address_valid(t_memory_mock* mock, void* address);

#endif