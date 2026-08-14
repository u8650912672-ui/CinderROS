#include <stdint.h>
#include <io.h>

struct idt_entry {
    uint16_t off_lo;
    uint16_t sel;
    uint8_t  ist;
    uint8_t  attr;
    uint16_t off_mid;
    uint32_t off_hi;
    uint32_t zero;
} __attribute__((packed));
__attribute__((aligned(16)))
static struct idt_entry idt[256];
struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern void isr_stub(void);

static void idt_set_gate(uint8_t n, uint64_t h) {
    idt[n].off_lo = h & 0xFFFF;
    idt[n].off_mid = (h >> 16) & 0xFFFF;
    idt[n].off_hi = (h >> 32) & 0xFFFFFFFF;
    idt[n].sel = 0x08; // 64 bit code segment :3 uwu~
    idt[n].ist = 0;
    idt[n].attr = 0x8E; //present ring 0 64 bit interrupter gate
    idt[n].zero = 0;
}
void irq_handler(void) { outb(0x20, 0x20); }
void idt_init(void) {
    for (int i = 0; i < 256; i++)
        idt_set_gate(i, (uint64_t)isr_stub); // this means every vector is the same stub
    struct idt_ptr p = { sizeof(idt) - 1, (uint64_t)idt };
    __asm__ volatile("lidt %0" :: "m"(p));
}

