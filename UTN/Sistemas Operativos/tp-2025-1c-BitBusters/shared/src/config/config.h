#ifndef CONFIG_H
#define CONFIG_H

#include <commons/config.h>

t_config* load_config(char* MODULE_CONFIG);
char* search_port(t_config* config, char* PORT_KEY);
char* search_ip(t_config* config, char* IP);
char* config_get_string(t_config* config, char* key);
double config_get_double(t_config* config, char* key);
int config_get_int(t_config* config, char* key);

#endif // CONFIG_H