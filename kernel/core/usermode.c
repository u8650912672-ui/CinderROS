#include <kernel.h>

extern void enter_ring3(uint64_t rip, uint64_t rsp);
extern void ring3_entry(void);

static uint8_t user_stack[16384] __attribute__((aligned(4096)));

void enter_userspace(void) {
    map_user_range((uint64_t)ring3_entry, 4096); // Allow userspace access to the code
    map_user_range((uint64_t)user_stack, sizeof(user_stack)); // Allow userspace access to the stack
    enter_ring3((uint64_t)ring3_entry, (uint64_t)&user_stack[sizeof(user_stack)]);
}
