#include <signal.h>
#include <servers/dispatch_listener.h>
#include <servers/interrupt_listener.h>
#include <servers/io_listener.h>
#include <connections/memory_connection.h>
#include <context/k_context.h>

#define SHORT_TERM_SCHEDULER_H

t_pcb* select_from_queue(t_list* queue, const char* alg);
void* short_term_scheduler(void* context);
int cmp_estburst(void* a, void* b);
int cmp_remaining(void* a, void* b); 
int find_index_short(t_list* list, bool (*condition)(void*, void*), void* context);
bool match_pcb_short(void* element, void* context);
int find_index_min(t_list* list, int (*cmp)(void* a, void* b));
void send_interrupt_to(int cpu_fd);
void send_interrupt_all(kernel_context_t* ctx);