BITS 64
section .text
extern irq_handler
global isr_stub
isr_stub:
    cli
    call irq_handler
    iretq