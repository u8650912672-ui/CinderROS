#include <stdint.h>
#include <kernel.h>

static struct mb2_tag_fb {
    uint32_t type;
    uint32_t size;
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t fb_type;
    uint16_t reserved;
} __attribute__((packed));

int mb2_get_framebuffer(uint64_t info, struct fb_info *out) {
    uint32_t base = (uint32_t)info; //mb2 ifno lives in low memo its identity mapped
    uint32_t end = base + *(uint32_t *)base; //total size is the first u32
    uint32_t off = base + 8; //tags start after the head (i need head :3)
    while (off + 8 <= end) {
        uint32_t type = *(uint32_t *)off;
        uint32_t size = *(uint32_t *)(off + 4);
        if (type == 0) break; //end tag
        if (type == 8 && size >= 32) { // frame buffer tag
            struct mb2_tag_fb *t = (struct mb2_tag_fb *)off;
            out->addr = t->addr;
            out->pitch = t->pitch;
            out->width = t->width;
            out->height = t->height;
            out->bpp = t->bpp;
            out->present = 1;
            return 0;
        }
        off += (size + 7) & ~7; //tags are 8 byte aligned
    }
    out->present = 0;
    return 1;
}
int mb2_get_module(uint64_t info, uint64_t *addr, uint64_t *size) {
    uint32_t base = (uint32_t)info;
    uint32_t end = base + *(uint32_t *)base;
    uint32_t off = base + 8;
    while (off + 8 <= end) {
        uint32_t type = *(uint32_t *)off;
        uint32_t sz = *(uint32_t *)(off + 4);
        if (type == 0) break;
        if (type == 3) {
            *addr = *(uint32_t *)(off + 8);
            *size = *(uint32_t *)(off + 12) - *(uint32_t *)(off + 8);
            return 0;
        }
        off += (sz + 7) & ~7;
    }
    return 1;
}