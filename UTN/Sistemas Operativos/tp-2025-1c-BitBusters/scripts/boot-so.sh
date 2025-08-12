#!/bin/bash

# Ruta base del proyecto
BASE_DIR="/home/utnso/tp-2025-1c-BitBusters/src"

# Función para abrir una nueva terminal, compilar y ejecutar
launch_terminal() {
  local MODULE_NAME=$1
  local DIR=$2
  local CMD=$3

  xfce4-terminal --title="$MODULE_NAME" --hold -e "bash -c 'cd $DIR && make && echo \"→ Ejecutando: $CMD\" && $CMD'"
}

# 1. Levantar MEMORIA y esperar 1 segundo
launch_terminal "MEMORIA" "$BASE_DIR/memoria" "./bin/memoria" &
sleep 0.2

# 2. Levantar KERNEL y esperar 3 segundos
launch_terminal "KERNEL" "$BASE_DIR/kernel" "./bin/kernel" &
sleep 0.2

# 3. Levantar IO
launch_terminal "IO" "$BASE_DIR/io" "./bin/io disco" &

# 4. Levantar CPU
launch_terminal "CPU" "$BASE_DIR/cpu" "./bin/cpu 1" &
