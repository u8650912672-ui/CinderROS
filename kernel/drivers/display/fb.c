#include <stdint.h>
#include <kernel.h>
extern const unsigned char font_data[];
static volatile uint8_t *fb;
static uint32_t width, height, pitch;
static uint8_t bpp_bytes;
static uint32_t col = 0, row = 0;
static int active = 0;
int fb_active(void) { return active; }

static void fb_pixel(uint32_t x, uint32_t y, int on) {
    uint8_t v = on ? 0xFF : 0x00;
    volatile uint8_t *p = fb + (uint64_t)y * pitch + (uint64_t)x * bpp_bytes;
    for (uint8_t i = 0; i < bpp_bytes; i++) p[i] = v;
}

static void fb_draw_char_at(unsigned char c, uint32_t x, uint32_t y) {
    const unsigned char *g = &font_data[c * 16];
    for (uint32_t r = 0; r < 16; r++)
        for (uint32_t xx = 0; xx < 8; xx++)
            fb_pixel(x + xx, y + r, (g[r] >> (7 - xx)) & 1);
}

void fb_clear(void) {
    if (!fb) return;
    for (uint64_t i = 0; i < (uint64_t)pitch * height; i++) fb[i] = 0;
    col = row = 0;
}
void fb_init(uint32_t w, uint32_t h, uint32_t p, uint8_t bpp, uint64_t addr) {
    if (!w || !h || !p || !addr || w < 8 || h < 32) return;
    fb = (volatile uint8_t *)(uintptr_t)addr;
    width = w; height = h; pitch = p;
    bpp_bytes = bpp / 8;
    if (!bpp_bytes) return;
    active = 0;
    fb_clear();
    active = 1;
}

static void fb_scroll(void) {
    uint64_t line = (uint64_t)pitch * 16;
    for (uint64_t i = 0; i + line < (uint64_t)pitch * height; i++)
        fb[i] = fb[i + line];
    for (uint64_t i = (uint64_t)pitch * (height - 16); i < (uint64_t)pitch * height; i++)
        fb[i] = 0;
    row--;
}

void fb_putchar(char c) {
    if (!active) return;
    if (c == '\n') {
        col = 0; row++;
    } else if (c == '\t') {
        do { fb_putchar(' '); } while (col % 8 != 0);
        return;
    } else if (c == '\b') {
        if (col > 0) col--;
        else if (row > 0) { row--; col = width / 8 - 1; }
        fb_draw_char_at(' ', col * 8, row * 16);
        return;
    } else {
        fb_draw_char_at((unsigned char)c, col * 8, row * 16);
        col++;
    }
    if (col >= width / 8) { col = 0; row++; }
    if (row >= height / 16) fb_scroll();
}