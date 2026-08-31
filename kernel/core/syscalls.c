#include <kernel.h>

uint64_t syscall_dispatch(uint64_t num, uint64_t a1, uint64_t a2, uint64_t a3) {
    switch (num) {
        case 0: dprint((const char*)a1); return 0;
        case 1: return (uint64_t)keyboard_getc();
        case 2: dclear(); return 0;
        default: return (uint64_t)-1;
    }
}