#ifndef KERNEL_H
#define KERNEL_H

void vga_clear(void);
void vga_putchar(char c);
void vga_print(const char *s);
void keyboard_init(void);
char keyboard_getc(void);
void idt_init(void);
void pic_init(void);
void kbd_irq(void);
unsigned long kbd_irqs(void);
int kbd_pending(void);
void ser_init(void);
void ser_putc(char);
void ser_print(const char *s);
void printf(const char *fmt, ...);
#endif
