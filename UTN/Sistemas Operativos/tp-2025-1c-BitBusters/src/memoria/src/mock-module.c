#include "mock-module.h"


extern t_log* logger;


t_memory_mock* create_mock_memory(size_t total_size, size_t block_size){
    t_config* config = config_create( "../config/memory.config");
    if(!config) {
        log_error(logger, "No se pudo cargar la configuración");
        return NULL;
    }

    //load config
    size_t memory_size = config_get_int_value(config, "TAM_MEMORIA");
    size_t page_size = config_get_int_value(config, "TAM_PAGINA");
    int levels = config_get_int_value(config, "CANTIDAD_NIVELES");
    int table_inputs = config_get_int_value(config, "ENTRADAS_POR_TABLA");

    t_memory_mock* mock = malloc(sizeof(t_memory_mock));
    if(!mock) {
        config_destroy(config);
        return NULL;
    }

    //inicializing structure
    mock->total_size = memory_size;
    mock->base = malloc(memory_size);
    if(!mock->base) {
        free(mock);
        config_destroy(config);
        return NULL;
    }

    //slots count
    mock->total_slots = memory_size / block_size;
    mock->free_slots = calloc(mock->total_slots, sizeof(bool));
    if(!mock->free_slots) {
        free(mock->base);
        free(mock);
        config_destroy(config);
        return NULL;
    }

    //pagination structure
    mock->pagination = int_paging_scheme(levels, table_inputs, page_size);
    if(!mock->pagination) {
        free(mock->free_slots);
        free(mock->base);
        free(mock);
        config_destroy(config);
        return NULL;
    }

    mock->process_table = dictionary_create();
    if(!mock->process_table) {
        destroy_pagination(mock->pagination);
        free(mock->free_slots);
        free(mock->base);
        free(mock);
        config_destroy(config);
        return NULL;
    }

    config_destroy(config);
    return mock;
}

void* memory_mock_alloc(t_memory_mock* mock, int pid,uint32_t virtual_addr) {
    if (!mock || !mock->pagination) return NULL;

    int frame = get_free_frame(mock->free_slots, mock->total_slots);
    if (frame == -1) return NULL;  // no free slots

    void* physical_addr = mock->base + (frame * mock->pagination->page_size);
    // Aquí iría la lógica para actualizar las tablas de página
    return physical_addr;
}


void memory_mock_free(t_memory_mock* mock, void* address) {
    if (!mock || !address) return;
    size_t offset = (char*)address - (char*)mock->base;
    size_t frame = offset / mock->pagination->page_size;

    if (frame < mock->total_slots) {
        mock->free_slots[frame] = false;
    }
}


void destroy_memory_mock(t_memory_mock* mock) {
    if (!mock) return;

    if (mock->process_table) dictionary_destroy(mock->process_table);
    if (mock->pagination) destroy_pagination(mock->pagination);
    free(mock->free_slots);
    free(mock->base);
    free(mock);
}

void memory_mock_dump(t_memory_mock* mock, int pid){
	if(!mock || !logger) {
		log_error(logger, "Mock o logger son null");
	}

    char filename[256];
	time_t current_time = time(NULL);

	t_config* config = config_create("../config/memory.config");
	if(!config) {
		log_error(logger, "No se pudo abrir el archivo de configuración");
	}

	const char* dump_path = config_get_string_value(config, "DUMP_PATH");
	if(!dump_path) {
		log_error(logger, "No se encontró DUMP_PATH en la configuración");
		config_destroy(config);
		return;
	}

	snprintf(filename,sizeof(filename),"<%s%d>-<%ld>.dmp", dump_path, pid, current_time);

	FILE* dump_file = fopen(filename, "wb");
	if(!dump_file) {
		log_error(logger, "No se pudo crear el archivo de dump: %s", filename);
		config_destroy(config);
		return;
	}

	size_t bytes_written = fwrite(mock->base, 1, mock->total_size, dump_file);
	if(bytes_written != mock->size) {
		log_error(logger, "Error al escribir el dump de memoria");
	} else{
		log_info(logger, "Dump de memoria creado correctamente en: %s", filename);
	}

	fclose(dump_file);
	config_destroy(config);
}