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
extern void isr_stub_32(void); //the stub the macro do
extern void isr_stub_33(void); //keyboard
extern void isr_syscall(void);

void pit_irq(void);
void kbd_irq(void);
void pic_eoi(int irq);

void irq_handler(uint64_t vector) {
    if (vector == 32) pit_irq();
    else if (vector == 33) kbd_irq();
    pic_eoi(vector - 32);
}
extern void isr_stub_0(void); extern void isr_stub_6(void); extern void isr_stub_8(void); extern void isr_stub_13(void); extern void isr_stub_14(void);
extern void isr_stub_13(void); extern void isr_stub_14(void);
void idt_init(void){
    for(int i=0;i<256;i++) idt_set_gate(i,(uint64_t)isr_stub_32);
    idt_set_gate(0,(uint64_t)isr_stub_0); idt_set_gate(6,(uint64_t)isr_stub_6); idt_set_gate(8,(uint64_t)isr_stub_8); idt_set_gate(13,(uint64_t)isr_stub_13); idt_set_gate(14,(uint64_t)isr_stub_14);
    idt_set_gate(33,(uint64_t)isr_stub_33);
    idt_set_gate(0x80,(uint64_t)isr_syscall);
    idt[0x80].attr |=0x60; // note cuz i will forget it the DPL3 MUST BE LAST or loop will overwrite it and ring3 int 0x80 will #GP err 402 cuz of the 0x80 vector IDT
    
}