#ifndef KERNEL_H
#define KERNEL_H

void vga_clear(void);
void vga_putchar(char c);
void vga_print(const char *s);

char keyboard_getc(void);
#endif
