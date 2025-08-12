#include "instruction_cycle.h"
#include <arpa/inet.h>

extern t_log *logger;
static bool running = true;
int interruptIdentificador = -1;

char *fetch(int memory_socket, int pc)
{
    log_info(logger, "Enviando PC=%d a memoria", pc); // LOG antes de enviar el PC

    int pc_net = htonl(pc);
    if (send(memory_socket, &pc_net, sizeof(int), 0) == -1)
    {
        log_error(logger, "Error al enviar PC a memoria");
        return NULL;
    }

    char *buffer = malloc(256);
    if (!buffer)
    {
        log_error(logger, "No se pudo asignar buffer para la instrucción");
        return NULL;
    }

    int bytes_received = recv(memory_socket, buffer, 255, 0);
    if (bytes_received <= 0)
    {
        log_error(logger, "Error al recibir instrucción de memoria (bytes_recibidos = %d)", bytes_received);
        free(buffer);
        return NULL;
    }

    buffer[bytes_received] = '\0';                        // Asegura null-terminado
    log_info(logger, "Instrucción recibida: %s", buffer); // LOG después de recibir

    return buffer;
}

struct opcode decode(char *instructionStr)
{
    struct opcode op;
    char *copia = strdup(instructionStr);
    char *token = strtok(copia, " ");

    if (token != NULL)
    {
        strncpy(op.instruction, token, sizeof(op.instruction) - 1);
        op.instruction[sizeof(op.instruction) - 1] = '\0';
    }

    int i = 0;
    while ((token = strtok(NULL, " ")) != NULL && i < 5)
    {
        op.operands[i] = strdup(token);
        i++;
    }

    for (int j = i; j < 5; j++)
    {
        op.operands[j] = NULL;
    }

    free(copia);
    return op;
}

void free_opcode(struct opcode *op)
{
    for (int i = 0; i < 5; i++)
    {
        if (op->operands[i] != NULL)
        {
            free(op->operands[i]);
            op->operands[i] = NULL;
        }
    }
}

void execute(struct opcode op, int memory_socket, int kernel_dispatch_socket, int kernel_interrupt_socket, k_message *k_message)
{

    int code = -1;
    if (strcmp(op.instruction, "NOOP") == 0)
    {
        code = 0;
    }
    else if (strcmp(op.instruction, "WRITE") == 0)
    {
        code = 1;
    }
    else if (strcmp(op.instruction, "READ") == 0)
    {
        code = 2;
    }
    else if (strcmp(op.instruction, "GOTO") == 0)
    {
        code = 3;
    }
    else if (strcmp(op.instruction, "IO") == 0)
    {
        code = 4;
        interruptIdentificador = 1;
    }
    else if (strcmp(op.instruction, "INIT_PROC") == 0)
    {
        code = 5;
        interruptIdentificador = 2;
    }
    else if (strcmp(op.instruction, "DUMP_MEMORY") == 0)
    {
        code = 6;
        interruptIdentificador = 3;
    }
    else if (strcmp(op.instruction, "EXIT") == 0)
    {
        code = 7;
        interruptIdentificador = 4;
    }

    switch (code)
    {
    case 0:
        execute_noop();
        k_message->pc++;
        break;
    case 1:
        execute_write(op, memory_socket);
        k_message->pc++;
        break;
    case 2:
        execute_read(op, memory_socket);
        k_message->pc++;
        break;
    case 3:
        execute_goto(op, k_message);
        k_message->pc++;
        break;
    case 4:
        execute_io(op, k_message->pid, kernel_interrupt_socket);
        k_message->pc++;
        break;
    case 5:
        execute_init_proc(op, k_message->process_size);
        k_message->pc++;
        break;
    case 6:
        execute_dump_memory();
        k_message->pc++;
        break;
    case 7:
        execute_exit(kernel_dispatch_socket, k_message);
        break;
    default:
        printf("Instrucción desconocida: %s\n", op.instruction);
        k_message->pc++;
        break;
    }
}

void execute_exit(int kernel_dispatch_socket, k_message *k_message)
{
    printf("EXIT - Finalizando proceso\n");
    running = false;
}

bool instruction_cycle(k_message *k_message, int memory_socket, int kernel_dispatch_socket, int kernel_interrupt_socket)
{
    if (!running)
        return false;

    log_info(logger, "Ciclo de instrucción para PID=%d, PC=%d", k_message->pid, k_message->pc);

    char *instruction = fetch(memory_socket, k_message->pc);
    if (!instruction || strlen(instruction) == 0)
    {
        if (instruction)
            free(instruction);
        log_error(logger, "No se pudo obtener la instrucción, o la instrucción está vacía");
        return false;
    }

    struct opcode op = decode(instruction);
    // bool is_exit = strcmp(op.instruction, "EXIT") == 0;

    execute(op, memory_socket, kernel_dispatch_socket, kernel_interrupt_socket, k_message);

    free(instruction);
    free_opcode(&op);

    int pidInterrup = k_message->pid;
    int pcInterrup = k_message->pc;

    uint32_t code;
    recv(kernel_interrupt_socket, &code, sizeof(code), 0);
    interruptIdentificador = ntohl(code);

    checkInterrupt(interruptIdentificador, kernel_interrupt_socket, pidInterrup, pcInterrup);

    return true;
}

bool checkInterrupt(int interruptIdentificador, int kernel_interrupt_socket, int pidInterrup, int pcInterrup)
{

    switch (interruptIdentificador)
    {
    case 5:
        log_error(logger, "Interrupción por desalojo SRT");
        break;
    case 1:
        log_error(logger, "Interrupción IO");
        break;
    case 2:
        log_error(logger, "Interrupción INIT_PROC");
        break;
    case 3:
        log_error(logger, "Interrupción DUMP_MEMORY");
        break;
    case 4:
        log_error(logger, "Interrupción EXIT");
        break;
    default:
        log_warning(logger, "No se reconoció interrupción ");
        return false;
    }

    send(kernel_interrupt_socket, &pidInterrup, sizeof(pidInterrup), 0);
    send(kernel_interrupt_socket, &pcInterrup, sizeof(pcInterrup), 0);
    return true;
}
