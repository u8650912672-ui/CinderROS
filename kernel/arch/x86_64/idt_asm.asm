BITS 64
section .text
extern irq_handler
global isr_stub
isr_stub:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    cli
    call irq_handler
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    iretq
    ;thanks jonathan the genius asm and keyboard helper