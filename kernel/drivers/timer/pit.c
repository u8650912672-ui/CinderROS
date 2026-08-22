#include <kernel.h>
#include <io.h>

static volatile uint64_t ticks = 0; //you know this shit will just keep going up :D
void pit_irq(void) {
    ticks++; //ticks go brrr
}
//HYPEER ADVANCEEEE CODEE INCOMING
uint64_t timer_ticks(void) {
    return ticks;
}
void pit_init(uint32_t freq) {
    uint32_t divisor = 1193182 / freq; //1.193182 MHZ
    outb(0x43, 0x36); //channel 0 pit channel 0
    outb(0x40, divisor & 0xFF); //low byte
    outb(0x40, (divisor >> 8) & 0xFF); //high byte :D
}