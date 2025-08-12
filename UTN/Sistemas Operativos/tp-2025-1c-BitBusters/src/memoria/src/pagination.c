#include "pagination.h"
#include "memory_manager.h"
#include <stdlib.h>
#include <math.h>
#include <commons/log.h>

extern t_log* logger;
static memory_manager_t* memory_manager = NULL;

void init_memory_manager(size_t memory_size, size_t page_size){
    memory_manager = malloc(sizeof(memory_manager_t));
    memory_manager->memory = malloc(memory_size);
    memory_manager->size = memory_size;
    memory_manager->page_size = page_size;
}

paging_scheme_t* int_paging_scheme(int levels, int entries_per_level, size_t page_size){
    paging_scheme_t* scheme = malloc(sizeof(paging_scheme_t));
    if(!scheme) {
        log_error(logger,"No se pudo crear el esquema de páginas");
        return NULL;
    }
    scheme->paging_levels = levels;
    scheme->entries_per_level = entries_per_level;
    scheme->page_size = page_size;
    scheme->root_table = create_page_table(levels - 1, entries_per_level);
    if(!scheme->root_table) {
        free(scheme);
        return NULL;
    }
    return scheme;
}

page_table_t* create_page_table(int level, int entry_count){
    page_table_t* table = malloc(sizeof(page_table_t));
    if(!table) {
        log_error(logger, "No se pudo crear la página");
        return NULL;
    }
    table->level = level;
    table->entry_count = entry_count;
    table->entries = calloc(entry_count, sizeof(page_table_entry_t));
    if(level > 0){
        table->next_table = calloc(entry_count, sizeof(page_table_t*));
    } else {
        table->next_table = NULL;
    }
    return table;
}

void* translate_address(paging_scheme_t* scheme, uint32_t virtual_addr){
    if(!scheme || !scheme->root_table) return NULL;
    page_table_t* current = scheme->root_table;
    int bits_per_level = log2(scheme->entries_per_level);
    int offset_bits    = log2(scheme->page_size);
    for(int lvl = scheme->paging_levels - 1; lvl >= 0; lvl--){
        int shift = offset_bits + (lvl * bits_per_level);
        int idx   = (virtual_addr >> shift) & (scheme->entries_per_level - 1);
        if(lvl == 0){
            if(!current->entries[idx].present) return NULL;
            uint32_t offset = virtual_addr & (scheme->page_size - 1);
            return (void*)(current->entries[idx].frame_number * scheme->page_size + offset);
        }
        if(!current->next_table[idx]) return NULL;
        current = current->next_table[idx];
    }
    return NULL;
}

static void free_page_table_recursive(page_table_t *table, int max_levels) {
    if(!table) return;
    if(table->level < max_levels - 1 && table->next_table) {
        for(int i = 0; i < table->entry_count; i++){
            if(table->next_table[i]){
                free_page_table_recursive(table->next_table[i], max_levels);
            }
        }
        free(table->next_table);
    }
    free(table->entries);
    free(table);
}

void free_paging_scheme(paging_scheme_t *scheme) {
    if(!scheme) return;
    free_page_table_recursive(scheme->root_table, scheme->paging_levels);
}

void destroy_pagination(paging_scheme_t *scheme) {
    if(!scheme) return;
    free_paging_scheme(scheme);
    free(scheme);
}

int get_free_frame(bool *free_slots, size_t total_slots) {
    for(size_t i = 0; i < total_slots; i++){
        if(free_slots[i]){
            free_slots[i] = false;
            return (int)i;
        }
    }
    return -1;
}