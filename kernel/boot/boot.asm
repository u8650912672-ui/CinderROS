;start of boot.asm
;Grub enters start in 32 bit proc mode with paging off i think so doign bits 32 should work
;also since this makes teh loader.asm and page_tables.asm useless ill remove them from this repo

BITS 32

section .multiboot
align 8
mb_header_start:
    dd 0xE85250D6 ;so called "magic"
    dd 0
    dd mb_header_end - mb_header_start ;this is the length
    dd -(0xE85250D6 + 0 + (mb_header_end - mb_header_start))
    dw 0 ;simple end tag
    dw 0
    dd 8
mb_header_end:
; also mb stands for multiboot for your info
section .boottext
global _start
extern kmain
extern __bss_start, __bss_end
gdt:
;gdt is painful cuz i have to FUCKING COUNT THE 32 BIT HEXBINARY BS
    dq 0x0000000000000000 ;same as dq 0 but its for visual
    dq 0x00AF9A000000FFFF ;0x08
    dq 0x00CF92000000FFFF 
gdt_end:
gdtr:
    dw gdt_end - gdt - 1
    dd gdt
_start:
    cli
    cld
    cmp eax, 0x36D76289
    jne .hang
    mov esp, stack_top
    
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    mov eax, PDP
    or eax, 0x3
    mov [PML4], eax
    mov eax, PD 
    or eax, 0x3
    mov [PDP], eax
    mov eax, 0x83
    xor ecx, ecx
.fill_pd:
    mov [PD + ecx * 8], eax
    add eax, 0x200000
    inc ecx
    cmp ecx, 64
    jb .fill_pd

    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    ;longmode enable (64 bit)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr
    ;here cr3 -> PML4
    mov eax, PML4
    mov cr3, eax
    ;then paging goes from off to on
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ; gdt -> long mode
    lgdt [gdtr]
    jmp 0x08:start64
.hang:
    cli
    hlt
    jmp .hang
;and now we enter 64 bit and leave shitty 32 behind
BITS 64
start64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, stack_top
    call kmain
.hang64:
    cli
    hlt
    jmp .hang64

    section .bss align=16
    stack_bottom:
        resb 16384
    stack_top:
    align 4096
PML4: resb 4096
PDP:  resb 4096
PD:   resb 4096