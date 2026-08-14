#define VGA_ADDR ((volatile unsigned short *)0xB8000)
#define VGA_COLS 80
#define VGA_ROWS 50

static int col = 0, row = 0;

void vga_clear(void) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++)
        VGA_ADDR[i] = 0x0720;
    col = row = 0;
}

void vga_putchar(char c) {
    if (c == '\n') { col = 0; row++; }
    else {
        VGA_ADDR[row * VGA_COLS + col] = (unsigned short)(0x0700 | (unsigned char)c);
        col++;
    }
    if (col >= VGA_COLS) { col = 0; row++; }
    if (row >= VGA_ROWS) row = VGA_ROWS - 1;
}
void vga_print(const char *s) {
    for (; *s; s++) vga_putchar(*s);
}