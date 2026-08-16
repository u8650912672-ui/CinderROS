#include <kernel.h>

void dputchar(char c) {
    if (fb_active()) fb_putchar(c);
    else vga_putchar(c);
}
void dprint(const char *s) {
    if (fb_active()) { while (*s) fb_putchar(*s++); }
    else vga_print(s);
}
void dclear(void) {
    if (fb_active()) fb_clear();
    else vga_clear();
}