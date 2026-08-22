BITS 64
section .text
extern irq_handler

%macro ISR_NOERR 1
global isr_stub_%1
isr_stub_%1:
    push 0 ;dummy cuz cpu wants it
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    mov rdi, %1
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
    add rsp, 8 ;bye bye error
    iretq ;ignore and walk away like nothin happend :)
%endmacro

ISR_NOERR 32
ISR_NOERR 33
    ;thanks jonathan the genius asm and keyboard helper