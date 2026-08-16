#include <kernel.h>
#include <io.h>

static int str_eq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}


__attribute__((noreturn))
void kmain(void) { //here da kernel starts this time it works
    vga_clear();
    ser_init();
    printf("CROS PRE-alpha stage :3 \n");
    printf("shell booted at %x (com1 serial: init'ed) \n", 0xDEADBEEF);
    vga_print("print clear for help wait no fuck help for clear WIAH AHHHH help for help and clear for clear \n");

    char line[128];
    int n = 0;
    keyboard_init(); // it will sit here like a good boy i threw it here cuz i need it somewhere and there were space here
    idt_init();
    pic_init();
    __asm__ volatile("sti");
    vga_print("good boys/girls type here ->");
    for (;;) {
        char c = keyboard_getc();
        if (c) {
            if (c == '\n') {
                vga_putchar('\n');
                line[n] = '\0';

                if (str_eq(line, "help"))
                    vga_print("commands: help, clear, uwu, shutdown\n");
                else if (str_eq(line, "clear"))
                    vga_clear();
                else if (str_eq(line, "femboy"))
                    vga_print("awe your a femboy? :3 good have some pats *pat pat pat*")
                else if (str_eq(line, "uwu"))
                    vga_print("Hello femboy *pat pat pat* tihi ;3\n");
                else if (str_eq(line, "shutdown")) {
                    outw(0xB004, 0x2000);  //THIS IS NOT FOR PROBLEMS THIS IS QEMU ONLY SHIT
                    for (;;) asm volatile("hlt");
                } else if (n > 0)
                    vga_print("thats not a command... mabye check fi your dyslexic :3?\n");
                
                n = 0;
                vga_print("# > ");
            }else if (c == '\b') {
                if (n > 0) { n--; vga_putchar('\b'); }
            }else if (n < 127) {
                line[n++] = c;
                vga_putchar(c);
            }
        }
        __asm__ volatile("pause");
    }
}
