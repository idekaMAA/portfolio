// #include <stdint.h>    // para uint32_t
// #include <stdlib.h>    // para NULL y malloc/free si las usas
// #include <math.h>      // para log2()
// #include "pagination.h"// para paging_scheme_t y page_table_t

// void* translate_adress(paging_scheme_t* scheme, uint32_t virtual_addr){
//     if(!scheme || !scheme->root_table) return NULL;

//     page_table_t* current_table = scheme->root_table;
//     int bits_per_level = log2(scheme->entries_per_table);
//     int offset_bits = log2(scheme->page_size);

//     for(int level = scheme->paging_levels -1; level >= 0; level--){
//         int shift = offset_bits + (level * bits_per_level);
//         int index = (virtual_addr >> shift) & (scheme->entries_per_level -1);

//         if(level == 0){
//             if (!current_table->entries[index].present){
//                 return NULL; //page fault
//             }
//             uint32_t offset = virtual_addr & (scheme->page_size -1);
//             return (void*)(current_table->entries[index].frame_number * scheme->page_size + offset);
//         }
//         if(!current_table->next_table[index]){
//             return NULL;
//         }
//         current_table = current_table->next_table[index];
//     }
//     return NULL;
// }. PASAR LAS STRUCTS A SHARED/STRUCTS