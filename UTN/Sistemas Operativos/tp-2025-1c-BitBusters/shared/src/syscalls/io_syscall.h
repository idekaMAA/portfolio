#ifndef IO_SYSCALL_H
#define IO_SYSCALL_H

typedef struct {
    int pid;
    int time;
    char device[64];
} t_syscall_io;

/**
 * @brief Crea una estructura de syscall_io
 * 
 * @param pid El identificador del proceso
 * @param device El nombre del dispositivo
 * @param time El tiempo asociado al syscall
 * @return t_syscall_io* Un puntero a la estructura creada
 */
t_syscall_io* syscall_io_create(int pid, const char* device, int time);

/**
 * @brief Libera la memoria de una estructura de syscall_io
 * 
 * @param io Un puntero a la estructura de syscall_io a liberar
 */
void syscall_io_destroy(t_syscall_io* io);

/**
 * @brief Serializa una estructura de syscall_io en un buffer
 * 
 * @param io Un puntero a la estructura de syscall_io a serializar
 * @param size Un puntero donde se almacenará el tamaño del buffer serializado
 * @return void* Un puntero al buffer serializado
 */
void* serialize_syscall_io(t_syscall_io* io, int* size);

/**
 * @brief Deserializa un buffer en una estructura de syscall_io
 * 
 * @param buffer Un puntero al buffer que contiene los datos serializados
 * @param buffer_size El tamaño del buffer
 * @return t_syscall_io* Un puntero a la estructura de syscall_io deserializada
 */
t_syscall_io* deserialize_syscall_io(void* buffer, int buffer_size);
#endif