#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>

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
void map_page_2m(uint64_t phys);
struct fb_info {
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    int present;
};
int mb2_get_framebuffer(uint64_t info, struct fb_info *out);
int gop_init(uint64_t mb2);
void fb_init(uint32_t w, uint32_t h, uint32_t pitch, uint8_t bpp, uint64_t addr);
void fb_putchar(char c);
void fb_clear(void);
int fb_active(void);
void dputchar(char c);
void dprint(const char *s);
void dclear(void);
void shell_run(void);
void shell_exec(const char *line);
int str_eq(const char *a, const char *b);
int mb2_get_module(uint64_t info, uint64_t *addr, uint64_t *size);
void ramfs_init(void);
int ramfs_mkdir(const char *name);
int ramfs_touch(const char *name);
void ramfs_ls(void);
int ramfs_cat(const char *name);
void ramfs_load_tar(uint64_t addr, uint64_t size);
int ramfs_rm(const char *name);int ramfs_rm(const char *name);
int ramfs_write(const char *name, const char *data, int len);
int acpi_shutdown(void);
void pit_init(uint32_t freq);
void pit_irq(void);
uint64_t timer_ticks(void);
void gdt_init(void);
#endif
