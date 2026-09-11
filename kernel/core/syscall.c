#include <kernel.h>
uint64_t syscall_handler(uint64_t num, uint64_t a1){ //num in rax, a1 in the rdi :3
    switch(num){
        case 0: dprint((const char*)a1); return 0; //easy and simple printer
        case 1: return (uint64_t)keyboard_getc(); //get c non blocking return 0 if key wrong
        case 2: return 0; //clear removed :3
        case 3: shell_exec((const char*)a1); return 0; //now just print no cmds
        case 4: __asm__ volatile("sti; hlt; cli"); return 0; //yied needs fix cuz ring0 ig?
        default: return (uint64_t)-1; //unknown syscall to call you stupid :D
    }
}
