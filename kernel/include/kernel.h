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
#endif
