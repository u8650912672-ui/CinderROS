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
extern fault_handler
%macro FAULT 1
global isr_stub_%1
isr_stub_%1:
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
    mov rsi, [rsp+72] ; MATH TIME erro code is at 64+8 after 8 pushes error alredy on stack for 13/14
    mov rdx, [rsp+96] ; rip is 8 more + CS/RFLAFGS and etc so its simpler to move jsut the rsi [rsp+72] brain died no more math
    call fault_handler ; hope and pray i guess
    cli
    hlt
    jmp $-2
%endmacro
FAULT 0
FAULT 6
FAULT 8
FAULT 13
FAULT 14
    ;thanks jonathan the genius asm and keyboard helper

extern syscall_handler
global isr_syscall
isr_syscall: 
    push rax ; saves the syscall number
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    mov rdi, [rsp + 72] ;sys call number sabed from rax
    mov rsi, [rsp + 32] ;user rdi (arg1)
    call syscall_handler
    mov [rsp + 72], rax ;return the value into saved rax
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq