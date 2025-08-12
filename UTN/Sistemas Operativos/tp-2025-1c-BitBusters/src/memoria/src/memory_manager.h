#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include "pagination.h"  // Necesario para paging_scheme_t

typedef struct {
    void* memory;                      // Memoria física simulada (malloc)
    size_t size;                       // Tamaño total de la memoria
    size_t page_size;                  // Tamaño de página
    paging_scheme_t* pagination;       // Esquema de paginación multinivel
} memory_manager_t;

// Prototipos posibles
memory_manager_t* create_memory_manager(size_t size, size_t page_size, int levels, int entries);
void free_memory_manager(memory_manager_t* manager);

#endif // MEMORY_MANAGER_H
