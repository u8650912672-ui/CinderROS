#include <io.h>
#include <stdint.h>
#define VGA_ADDR ((volatile uint16_t *)0xB8000)
#define VGA_COLS 80
#define VGA_ROWS 25
static int col = 0, row = 0;
static void vga_set_cursor(void) {
    uint16_t pos = row * VGA_COLS + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF)); }
static void vga_scroll(void) {
    for (int i = 0; i < (VGA_ROWS - 1) * VGA_COLS; i++)
        VGA_ADDR[i] = VGA_ADDR[i + VGA_COLS];
    for (int i = (VGA_ROWS - 1) * VGA_COLS; i < VGA_ROWS * VGA_COLS; i++)
        VGA_ADDR[i] = 0x0720;
    row = VGA_ROWS - 1; }
void vga_clear(void) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++)
        VGA_ADDR[i] = 0x0720;
    col = row = 0;
    vga_set_cursor(); }
void vga_putchar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    } else if (c == '\t') {
        do { vga_putchar(' '); } while (col % 8 != 0);
        return;
    } else if (c == '\b') {
        if (col > 0) col--;
        else if (row > 0) { row--; col = VGA_COLS - 1; }
        VGA_ADDR[row * VGA_COLS + col] = 0x0720;
    } else {
        VGA_ADDR[row * VGA_COLS + col] = (uint16_t)(0x0700 | (uint8_t)c);
        col++; }
    if (col >= VGA_COLS) { col = 0; row++; }
    if (row >= VGA_ROWS) vga_scroll();
    vga_set_cursor(); }
void vga_print(const char *s) {
    for (; *s; s++) vga_putchar(*s); }
