#include "execute.h"
#include <stdio.h>
#include <stdlib.h>
#include "context/k_message.h"
#include <string.h>
#include <sys/socket.h>
#include <syscalls/io_syscall.h>

extern t_log *logger;

static bool falta_parametros(struct opcode op, int cantidad)
{
    for (int i = 0; i < cantidad; i++)
    {
        if (op.operands[i] == NULL)
            return true;
    }
    return false;
}

void execute_noop(void)
{
    log_info(logger, "NO_OP ejecutado: sin operación");
}

void execute_write(struct opcode op, int memory_socket)
{
    if (falta_parametros(op, 2))
    {
        log_error(logger, "WRITE: faltan parámetros de direccion o datos");
        return;
    }

    int direccion_logica = atoi(op.operands[0]);
    char *datos = op.operands[1];
    int direccion_fisica = direccion_logica;
    int tamanio = strlen(datos) + 1;

    t_package *paquete = create_packet();
    paquete->operation_code = WRITE_OP; // Asegurate de definirlo en el enum

    add_to_packet(paquete, &direccion_logica, sizeof(int));
    add_to_packet(paquete, &tamanio, sizeof(int));
    add_to_packet(paquete, datos, tamanio);

    send_packet(paquete, memory_socket);
    log_info(logger, "WRITE enviado: dir=%d, datos='%s'", direccion_fisica, datos);

    delete_packet(paquete);
}

void execute_read(struct opcode op, int memory_socket)
{
    if (falta_parametros(op, 2))
    {
        log_error(logger, "READ: falta parámetro de dirección o tamaño");
        return;
    }

    int direccion_logica = atoi(op.operands[0]);
    int tamanio = atoi(op.operands[1]);
    int direccion_fisica = direccion_logica;

    t_package *paquete = create_packet();
    paquete->operation_code = READ_OP;

    add_to_packet(paquete, &direccion_logica, sizeof(int));
    add_to_packet(paquete, &tamanio, sizeof(int));

    send_packet(paquete, memory_socket);
    log_info(logger, "READ enviado: dir=%d, tamaño=%d", direccion_fisica, tamanio);
    
    // Esperar la respuesta del contenido leído
    char *buffer = malloc(tamanio);
    recv(memory_socket, buffer, tamanio, 0);

    log_info(logger, "Contenido leído: %s", buffer);
    printf("Contenido leído de memoria: %s\n", buffer);

    delete_packet(paquete);
}

void execute_goto(struct opcode op, k_message *k_msg)
{
    if (falta_parametros(op, 1))
    {
        log_error(logger, "GOTO: falta dirección");
        return;
    }

    int nueva_direccion = atoi(op.operands[0]);
    k_msg->pc = nueva_direccion;

    log_info(logger, "GOTO ejecutado: PC actualizado a %d", nueva_direccion);
}

void execute_io(struct opcode op, int pid, int interrupt_socket)
{
    log_info(logger, "IO ejecutado");

    if (falta_parametros(op, 2))
    {
        log_error(logger, "IO: faltan operandos (device o tiempo)");
        return;
    }

    t_syscall_io syscall;
    syscall.pid = pid;
    syscall.time = atoi(op.operands[1]);

    strncpy(syscall.device, op.operands[0], sizeof(syscall.device) - 1);
    syscall.device[sizeof(syscall.device) - 1] = '\0'; // null-terminate

    int size;
    void *buffer = serialize_syscall_io(&syscall, &size);

    if (send(interrupt_socket, buffer, size, 0) == -1)
    {
        log_error(logger, "Error al enviar syscall IO al Kernel (interrupt_socket)");
    }
    else
    {
        log_info(logger, "Syscall IO enviada al Kernel: PID=%d, device=%s, time=%d ms",
                 syscall.pid, syscall.device, syscall.time);
    }

    free(buffer);
}

void execute_init_proc(struct opcode op, int socket_dispatch)
{
    if (falta_parametros(op, 2))
    {
        log_error(logger, "INIT_PROC: falta parámetro de archivo o tamaño");
        return;
    }
    char *archivo_instrucciones = op.operands[0];
    int tam_proceso = atoi(op.operands[1]);

    log_info(logger, "INIT_PROC ejecutado: archivo=%s, tamaño=%d", archivo_instrucciones, tam_proceso);
    INIT_PROC(socket_dispatch, tam_proceso);
}

void execute_dump_memory(void)
{
    log_info(logger, "DUMP_MEMORY solicitado");
}
