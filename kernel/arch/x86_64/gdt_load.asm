BITS 64
section .text 

global gdt_flush
gdt_flush:
    lgdt [rdi] ;rdi = pointer to gdt_ptr struct ofc :)
    mov ax, 0x10 ;ring 0 data 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    push 0x08 ;ring 0 code selecter
    lea rax, [rel .reload]
    push rax
    retfq ;far return reloads the CS pops full 8 bytes ;D
.reload:
    ret

global tss_flush
tss_flush:
    mov ax, 0x28 ;tss selector
    ltr ax
    ret
    ;and done :D
    

section .text
global enter_ring3
enter_ring3:
    mov rax, 0x20 | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push rax
    push rsi
    pushfq
    pop rax
    or rax, 0x200
    push rax
    push (0x18 | 3)
    push rdi
    iretq