#include <kernel.h>
#include <io.h>

struct tss {
    uint32_t reserved0;
    uint64_t rsp0; uint64_t rsp1; uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1; uint64_t ist2; uint64_t ist3;
    uint64_t ist4; uint64_t ist5; uint64_t ist6; uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
static struct tss tss;
static uint64_t gdt[10]; //5 entiries null code data

static void set_entry(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[i] = (limit & 0xFFFF)
            | ((uint64_t)(base & 0xFFFF) << 16)
            | ((uint64_t)((base >> 16) & 0xFF) << 32)
            | ((uint64_t)access << 40)
            | ((uint64_t)((limit >> 16) & 0x0F) << 48)
            | ((uint64_t)(flags & 0x0F) << 52)
            | ((uint64_t)((base >> 24) & 0xFF) << 56);
}
static void set_entry64(int i, uint64_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    set_entry(i, (uint32_t)base, limit, access, flags);
    gdt[i + 1] = base >> 32;
}
extern void gdt_flush(struct gdt_ptr *p);
extern void tss_flush(void);
void gdt_init(void) {
    for (int i = 0; i < 10; i++) gdt[i] = 0;

    set_entry64(1, 0, 0xFFFFF, 0x9A, 0xA); //ring 0 code (0x08)
    set_entry64(2, 0, 0xFFFFF, 0x92, 0xC); //ring 0 data (0x10)
    set_entry64(3, 0, 0xFFFFF, 0xFA, 0xA); //first ring 3 :) code (0x18)
    set_entry64(4, 0, 0xFFFFF, 0xF2, 0xC); //and then ring 3 data (0x20)
    set_entry64(5, (uint64_t)&tss, sizeof(struct tss) - 1, 0x89, 0x0); //then tss aka 0x28
    tss.iopb = sizeof(struct tss);
    tss.rsp0 = 0; //set this shit later when i got a realstack i can point
    struct gdt_ptr p = { sizeof(gdt) - 1, (uint64_t)&gdt };
    gdt_flush(&p);
    tss_flush();
}