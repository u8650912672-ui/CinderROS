#include <stdint.h>
#include <kernel.h>

extern const unsigned char font_data[]; //from font.asm and you can change this to whatever you want

static volatile uint8_t *fb;
static uint32_t pitch, height;
static uint8_t bpp_bytes;
static int col = 0, row = 0;
static int active = 0;
int fb_active(void) { return active; }
void fb_init(uint32_t w, uint32_t h, uint32_t p, uint8_t bpp, uint64_t addr) {
    (void)w;
    fb = (volatile uint8_t *)(uintptr_t)addr;
    pitch = p;
    height = h;
    bpp_bytes = bpp / 8;
    col = row = 0;
    active = 1;
    fb_clear();
}

static void fb_pixel(uint32_t x, uint32_t y, int on) {
    uint8_t v = on ? 0xFF : 0x00;
    volatile uint8_t *p = fb + (uint64_t)y * pitch + (uint64_t)x * bpp_bytes;
    for (uint8_t i = 0; i < bpp_bytes; i++) p[i] = v;
}

static void fb_draw_char_at(unsigned char c, uint32_t x, uint32_t y) {
    const unsigned char *g = &font_data[c * 16];  //16 rows per glyph
    for (uint32_t r = 0; r < 16; r++) {
        uint8_t bits = g[r]; //bit 7 lefmost
        for (uint32_t xx = 0; xx < 8; xx++)
            fb_pixel(x + xx, y + r, (bits >> (7 - xx)) & 1);
    }
}
void fb_clear(void) {
    for (uint64_t i = 0; i < (uint64_t)pitch * height; i++) fb[i] = 0;
    col = row = 0;
}
void fb_putchar(char c) {
    if (c == '\n') {
        col = 0; row++;
    } else if (c == '\t') {
        do { fb_putchar(' '); } while (col % 8 != 0);
        return;
    } else if (c == '\b') {
        if (col > 0) col--;
        else if (row > 0) { row--; col = (pitch / 8) - 1; }
        fb_draw_char_at(' ', col * 8, (uint32_t)row * 16); //erase :3
        return;
    } else {
        fb_draw_char_at((unsigned char)c, col * 8, (uint32_t)row * 16);
        col++;
    }
    if (col >= pitch / 8) { col = 0; row++; }
    if ((uint32_t)row >= height / 16) {
        uint64_t line = (uint64_t)pitch * 16;
        for (uint64_t i = 0; i + line < (uint64_t)pitch * height; i++)
            fb[i] = fb[i + line];
        uint64_t last = (uint64_t)pitch * (height - 16);
        for (uint64_t i = last; i < (uint64_t)pitch * height; i++)
            fb[i] = 0;
        row--;
    }
}