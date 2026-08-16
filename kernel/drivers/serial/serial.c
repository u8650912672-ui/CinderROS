#include <io.h>
#define COM1 0x3f8
void ser_init(void) {  //115200 baud
    outb(COM1 + 1, 0x00); //disable interrupts
    outb(COM1 + 3, 0x80); //Dlab on
    outb(COM1 + 0, 0x01); //divisor high (like how high i am)
    outb(COM1 + 1, 0x00); //divisor low
    outb(COM1 + 3, 0x03); // 8 bits dlab is off
    outb(COM1 + 2, 0xC7); // Fifo enable clear and trigger
    outb(COM1 + 4, 0x0B); //RTS/DSR set IRQ on
    //i just noticed i forgot to put ; ON EVERY SINGEL FUCKING ONE WHEN I DID THIS FIRST TIME 
    //HOLY SHIT no wounder it didnt fucking compile
}
void ser_putc(char c) {
    while (!(inb(COM1 + 5) & 0x20)); //wait thr empty
    outb(COM1, c); //also if anyone is woundering yes i am using a ref sheet tutorial and ai for help i am stupid okay?
}
void ser_print(const char *s) { while (*s) ser_putc(*s++); }
