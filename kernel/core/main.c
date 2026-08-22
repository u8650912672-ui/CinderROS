#include <kernel.h>
#include <io.h>
__attribute__((noreturn))
void kmain(uint64_t mb2) { //here da kernel starts this time it works
    dclear();
    ser_init();
    struct fb_info fb;
    if (!mb2_get_framebuffer(mb2, &fb) && (fb.bpp == 32 || fb.bpp == 24)) {
        uint64_t end = fb.addr + (uint64_t)fb.pitch * fb.height;
        for (uint64_t a = fb.addr; a < end; a += 0x200000)
            map_page_2m(a);
        fb_init(fb.width, fb.height, fb.pitch, fb.bpp, fb.addr);
    }
    
    printf("CROS PRE-alpha stage :3 \n");
    printf("shell booted at %x (com1 serial: init'ed) \n", 0xDEADBEEF);
    dprint("print clear for help wait no fuck help for clear WIAH AHHHH help for help and clear for clear \n");
    ramfs_init();
    uint64_t maddr = 0, msize = 0;
    if (mb2_get_module(mb2, &maddr, &msize) == 0)
        ramfs_load_tar(maddr, msize);
    gdt_init();
    keyboard_init();
    idt_init();
    pic_init();
    pit_init(100); //for easier math 100 hz at 1 second means 100 ticks in 1 second :)
    __asm__ volatile("sti");
    shell_run();
}