#include "config.h"
#include <commons/config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

t_config* load_config(char* MODULE_CONFIG) {
    return config_create(MODULE_CONFIG);
}

char* search_port(t_config* config, char* PORT_KEY) {
    return strdup(config_get_string_value(config, PORT_KEY));
}

char* search_ip(t_config* config, char* IP) {
    return strdup(config_get_string_value(config, IP));
}

// Obtiene un string de la configuración
char* config_get_string(t_config* config, char* key) {
    return strdup(config_get_string_value(config, key));
}

// Obtiene un double de la configuración
double config_get_double(t_config* config, char* key) {
    return atof(config_get_string_value(config, key));
}

// Obtiene un int de la configuración
int config_get_int(t_config* config, char* key) {
    return atoi(config_get_string_value(config, key));
}