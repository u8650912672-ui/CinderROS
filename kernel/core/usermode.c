#include <kernel.h>

extern void enter_ring3(uint64_t rip, uint64_t rsp);
static inline void sys_print(const char *s){ asm volatile("int $0x80"::"a"(0), "D"(s):"memory"); } //just prints lol 
static inline int sys_getc(void){ int r; asm volatile("int $0x80" : "=a"(r) : "a"(1) : "memory"); return r; } // grabs the key if there is a key
static inline void sys_exec(const char *s){ asm volatile("int $0x80"::"a"(3),"D"(s):"memory"); } //kernel is retarded tell to do idfk 
static inline void sys_hlt(void){ asm volatile("int $0x80"::"a"(4):"memory"); } //hlt fix i wanna die idk
__attribute__((section(".usercode")))
void user_shell_main(void){ //tell the real shell not to echo the bullshit
    char line[128]; //guess where i got this shit from 
    int n = 0;
    sys_print(" :3 userspace shell :3\n"); // ellooo my borhter ;3
    sys_print("# > ");
    for(;;){
        int c = sys_getc();
        if(!c){ asm volatile("pause"); continue; } // dont even call sys hlt just apuse D:
        if(c == '\n'){
            sys_print("\n");
            line[n] = '\0';
            if(n) sys_exec(line); //let kernel do lifting cuz i dont wanna reimplement commands.c here
            n = 0;
            sys_print("# > "); //ill add shit later 
        } else if(c == '\b'){
            if(n > 0){ n--; sys_print("\b"); } //delete 1 char like a good little boy :3
        } else if(n < 127){
            line[n++] = (char)c; //funny fella
            char tmp[2] = { (char)c, 0 };
            sys_print(tmp);
        }
    }
}
static uint8_t user_stack[16384] __attribute__((aligned(4096)));
void enter_userspace(void){
    map_user_range((uint64_t)user_shell_main, 4096);
    map_user_range((uint64_t)user_stack, sizeof(user_stack));
    enter_ring3((uint64_t)user_shell_main, (uint64_t)&user_stack[sizeof(user_stack)]);
}
