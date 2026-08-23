#include <kernel.h>

extern void enter_ring3(uint64_t rip, uint64_t rsp);
extern void ring3_entry(void);

static uint8_t user_stack[16384] __attribute__((aligned(16)));

void enter_userspace(void) {
    enter_ring3((uint64_t)ring3_entry, (uint64_t)&user_stack[sizeof(user_stack)]);
}
