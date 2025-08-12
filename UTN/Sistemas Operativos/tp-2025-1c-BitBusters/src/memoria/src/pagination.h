#ifndef PAGINATION_H
#define PAGINATION_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Estructuras básicas
struct page_table_entry {
    uint32_t frame_number : 20;
    uint8_t present : 1;
    uint8_t modified : 1;
    uint8_t used : 1;
};

// Declaración forward de la estructura
struct page_table {
    struct page_table_entry* entries;
    struct page_table** next_table;
    int level;
    int entry_count;
};

// Typedefs después de las declaraciones
typedef struct page_table_entry page_table_entry_t;
typedef struct page_table page_table_t;

// Esquema de paginación
typedef struct {
    struct page_table* root_table;
    int paging_levels;
    int entries_per_level;
    size_t page_size;
} paging_scheme_t;

// Prototipos de funciones
paging_scheme_t* int_paging_scheme(int levels, int entries_per_level, size_t page_size);
page_table_t* create_page_table(int level, int entry_count);
void* translate_addr(struct page_table* table, uint32_t virtual_addr);
void free_paging_scheme(paging_scheme_t* scheme);
int get_free_frame(bool *free_slots, size_t total_slots);
void destroy_pagination(paging_scheme_t* scheme);
void* translate_address(paging_scheme_t* scheme, uint32_t virtual_addr);





#endif // PAGINATION_H