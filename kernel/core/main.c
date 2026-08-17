#include <kernel.h>
#include <io.h>

static int str_eq(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

__attribute__((noreturn))
void kmain(uint64_t mb2) { //here da kernel starts this time it works
    
    dclear();
    ser_init();
    if (gop_init(mb2) != 0) {
        struct fb_info fb;
        if (!mb2_get_framebuffer(mb2, &fb) && (fb.bpp == 32 || fb.bpp == 24)) {
            uint64_t end = fb.addr + (uint64_t)fb.pitch * fb.height;
            for (uint64_t a = fb.addr; a < end; a += 0x200000)
                map_page_2m(a);
            fb_init(fb.width, fb.height, fb.pitch, fb.bpp, fb.addr);
            volatile uint8_t *raw = (volatile uint8_t *)(uintptr_t)fb.addr;
            for (uint64_t i = 0; i < 160000 && i < (uint64_t)fb.pitch * fb.height; i++)
                raw[i] = 0xFF;
        }
    }
    printf("CROS PRE-alpha stage :3 \n");
    printf("shell booted at %x (com1 serial: init'ed) \n", 0xDEADBEEF);
    dprint("print clear for help wait no fuck help for clear WIAH AHHHH help for help and clear for clear \n");

    char line[128];
    int n = 0;
    keyboard_init(); // it will sit here like a good boy i threw it here cuz i need it somewhere and there were space here
    idt_init();
    pic_init();
    __asm__ volatile("sti");
    dprint(" :3 good boys/girls type here ->");
    for (;;) {
        char c = keyboard_getc();
        if (c) {
            if (c == '\n') {
                dputchar('\n');
                line[n] = '\0';

                if (str_eq(line, "help"))
                    dprint("commands: help, clear, uwu, shutdown\n");
                else if (str_eq(line, "clear"))
                    dclear();
                else if (str_eq(line, "femboy"))
                    dprint("awe your a femboy? :3 good have some pats *pat pat pat*\n");
                else if (str_eq(line, "uwu"))
                    dprint("Hello femboy *pat pat pat* tihi ;3\n");
                else if (str_eq(line, "shutdown")) {
                    outw(0xB004, 0x2000);  //THIS IS NOT FOR PROBLEMS THIS IS QEMU ONLY SHIT
                    for (;;) asm volatile("hlt");
                } else if (n > 0)
                    dprint("thats not a command... mabye check fi your dyslexic :3?\n");
                
                n = 0;
                dprint("# > ");
            }else if (c == '\b') {
                if (n > 0) { n--; dputchar('\b'); }
            }else if (n < 127) {
                line[n++] = c;
                dputchar(c);
            }
        }
        __asm__ volatile("pause");
    }
}
