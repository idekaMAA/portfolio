#include <signal.h>
#include <servers/dispatch_listener.h>
#include <servers/interrupt_listener.h>
#include <servers/io_listener.h>
#include <connections/memory_connection.h>
#include <context/k_context.h>

#define LONG_TERM_SCHEDULER_H

void* long_term_scheduler(void* context);
int cmp_memsize(void* a, void* b);
t_pcb* select_from_new(kernel_context_t* ctx);
void enqueue_ready(kernel_context_t* ctx, t_pcb* p);
void signal_memory_freed(kernel_context_t* ctx);