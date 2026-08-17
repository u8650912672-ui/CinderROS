#include <stdint.h>
//this is for GOP aka UEFI text framebuffer kinda like VBE but UEFI
#include <kernel.h>
typedef uint64_t (*lp_t)(uint64_t rcx, uint64_t rdx, uint64_t r8) __attribute__((ms_abi));
static const unsigned char gop_guid[16] = {0xDE,0xA9,0x42,0x90,0xDC,0x23,0x38,0x4A,
                                           0x96,0xFB,0x7A,0xDE,0xD0,0x80,0x51,0x6A};

int gop_init(uint64_t info) {
    uint32_t base = (uint32_t)info;
    uint32_t end = base + *(uint32_t *)base;
    uint32_t off = base + 8;
    uint64_t st = 0;
    while (off + 8 <= end) {
        uint32_t type = *(uint32_t *)off;
        uint32_t size = *(uint32_t *)(off + 4);
        if (type == 0) break;
        if (type == 12 && size >= 24) { st = *(uint64_t *)(off + 8); break; }
        off += (size + 7) & ~7;
    }
    if (!st) return 1;

    for (uint64_t a = 0; a < 0x100000000ULL; a += 0x200000)
        map_page_2m(a);

    uint64_t bs = *(uint64_t *)(st + 0x58);
    lp_t locate = (lp_t)(*(uint64_t *)(bs + 0x140));
    uint64_t gop = 0;
    if (locate((uint64_t)&gop_guid, 0, (uint64_t)&gop) != 0 || !gop)
        return 1;
    uint64_t mode = *(uint64_t *)(gop + 0x18);
    uint64_t mi = *(uint64_t *)(mode + 0x20);
    uint32_t w = *(uint32_t *)(mi + 0x04);
    uint32_t h = *(uint32_t *)(mi + 0x08);
    uint64_t fbb = *(uint64_t *)(mode + 0x30);
    uint64_t fbs = *(uint64_t *)(mode + 0x38);
    if (!fbb || !w || !h || fbs < (uint64_t)w * h * 4)
        return 1;
    uint32_t pitch = (uint32_t)(fbs / h);
    if (pitch < w * 4)
        return 1;
    fb_init(w, h, pitch, 32, fbb);
    volatile uint8_t *raw = (volatile uint8_t *)(uintptr_t)fbb;
    for (uint64_t i = 0; i < 160000 && i < fbs; i++)
        raw[i] = 0xFF;
    return 0;
}
