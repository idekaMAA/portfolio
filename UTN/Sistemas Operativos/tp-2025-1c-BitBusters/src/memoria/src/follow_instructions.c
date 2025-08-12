#include "follow_instructions.h"
#include "server.h"
#include "mock-module.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <structs/memory-cpu/op_code.h>

extern t_log* logger;

void handle_execute_instruction(int client_socket) {
    execute_instruction_request_t req;
    ssize_t received = recv_all(client_socket, &req, sizeof(execute_instruction_request_t));

    if (received != sizeof(execute_instruction_request_t)) {
        log_error(logger, "No se pudo recibir la solicitud EXEC_INSTRUCCION");
        return;
    }

    log_info(logger, "EXEC_INSTRUCCION recibido: PID=%d, instrucción='%s'", req.pid, req.instruction);
    
    if (strcmp(req.instruction, "WRITE") == 0) {
        write_request_t write_req;
        if (recv_all(client_socket, &write_req, sizeof(write_request_t)) != sizeof(write_request_t)) {
            log_error(logger, "No se pudo recibir la estructura WRITE");
            return;
        }

        memory_mock_write(req.pid, write_req.physical_address, write_req.data);

    } else if (strcmp(req.instruction, "READ") == 0) {
        read_request_t read_req;
        if (recv_all(client_socket, &read_req, sizeof(read_request_t)) != sizeof(read_request_t)) {
            log_error(logger, "No se pudo recibir la estructura READ");
            return;
        }
        
        read_response_t response;
        memory_mock_read(req.pid, read_req.physical_address, read_req.size);

        send(client_socket, &response, sizeof(read_response_t), 0);
    } else {
        log_info(logger, "Instrucción no reconocida o sin efectos en memoria: %s", req.instruction);
    }

    // Confirmación al CPU
    char* response = "INSTRUCCION_EJECUTADA";
    send(client_socket, response, strlen(response) + 1, 0);
}