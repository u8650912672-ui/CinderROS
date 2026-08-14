void vga_clear(void);
void vga_print(const char *s);

__attribute__((section(".text.entry"), noreturn))
void _start(void) { // here da kernel starts havent been written
    vga_clear();
    vga_print("Hello Femboys~");
    for (;;) asm volatile("hlt");
}