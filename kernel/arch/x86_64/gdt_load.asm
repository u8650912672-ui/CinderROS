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
    
section .usercode align=4096 ; Put code in own section and page align it so that we can set the memory permissions easily
global ring3_entry
ring3_entry:
    mov rax, 0 ; ID for the test syscall
    int 0x80
.spin:
    ; cli/hlt are ring 0 only, just spin
    jmp .spin

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