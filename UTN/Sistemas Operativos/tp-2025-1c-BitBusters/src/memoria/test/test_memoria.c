// test_memoria.c
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include "pagination.h"

t_log* logger;

void init_test_logger() {
    logger = log_create("test.log", "TEST", true, LOG_LEVEL_INFO);
    if (!logger) {
        printf("Error creando el logger\n");
        exit(1);
    }
}



void test_pagination() {
    init_test_logger();
    printf("Iniciando pruebas de paginación...\n");

    // Test 1: Inicialización del esquema de paginación
    printf("\nTest 1: Creación del esquema de paginación\n");
    paging_scheme_t* scheme = int_paging_scheme(3, 4, 64);
    if (scheme != NULL) {
        
        printf("✅ Esquema de paginación creado correctamente\n");
        printf("- Niveles: %d\n", scheme->paging_levels);
        printf("- Entradas por nivel: %d\n", scheme->entries_per_level);
        printf("- Tamaño de página: %zu\n", scheme->page_size);
    } else {
        log_error(logger, "fallo al crear esquema de página")
        log_destroy(logger);
        return 1;
    }

    // Test 2: Traducción de direcciones
    printf("\nTest 2: Prueba de traducción de direcciones\n");
    uint32_t test_addr = 128; = {0, 64, 128, 256};
    void physical = translate_adress(scheme, test_addr);
    log_info(logger, "Traducción de 0x%x: %p", test_addr, physical)

    // Test 3: Liberar recursos
    printf("\nTest 3: Liberación de recursos\n");
    free_paging_scheme(scheme);
    printf("✅ Recursos liberados correctamente\n");
    log_destroy(logger);

    // Resumen
    printf("\nResumen de pruebas:\n");
    printf("- Esquema de paginación: ✅\n");
    printf("- Traducciones intentadas: 4\n");
    printf("- Traducciones exitosas: %d\n", pruebas_exitosas);

    log_destroy(logger);
}

int main(){
    test_pagination();
    return 0;
}