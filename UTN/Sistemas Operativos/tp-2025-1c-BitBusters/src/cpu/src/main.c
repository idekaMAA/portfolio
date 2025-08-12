#include <run/run-cpu.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <cpu_id>\n", argv[0]);
        return EXIT_FAILURE;
    }

    RUN_CPU(argv[1]);
    return EXIT_SUCCESS;
}