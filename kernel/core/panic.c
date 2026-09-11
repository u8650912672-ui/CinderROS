#include <kernel.h>
void fault_handler(uint64_t vec, uint64_t err, uint64_t rip){ //lol rip hah :D
    printf("\nFAULT vec %d err %x rip %x :3\n", (int)vec, (int)err, (int)rip);
    dprint("if you see this open a ticket with your specs and this and ill try decode it or you will be ignored\n");
    for(;;) __asm__ volatile("cli; hlt");    
}