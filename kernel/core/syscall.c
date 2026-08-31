#include <kernel.h>

uint64_t syscall_handler(uint64_t num) {
    // Temporary test syscall
    if (num == 0) {
        printf("syscall: test syscall hit :3\n");
        return 0x1234;
    }

    printf("syscall: unknown number %d\n", (int)num);
    
    return (uint64_t)-1;
}
