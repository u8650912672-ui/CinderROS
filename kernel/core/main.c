__attribute__((section(".text.entry"), noreturn))
void _start(void) { // here da kernel starts havent been written
    const char msg[] = "CROS";
    volatile char *fb = (volatile char *)0xB8000;
    for (int i = 0; msg[i]; i++) {
        fb[i * 2] = msg[i];
        fb[i * 2 + 1] = 0x0F;
    }
    for (;;) asm volatile("hlt");
}
