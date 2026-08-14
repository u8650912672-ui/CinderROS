;stage 2 loaded from start/stage1.asm

[bits 16]
[org 0x7E00]

;save boot drive
    mov [boot_drive], dl
    cli
    mov al, 'A'
    call printch

;enable A20 line
enable_a20:
;try a buis fync
    mov ax, 0x2401
    int 0x15
    jnc .a20_ok

    in al, 0x92
    test al, 2
    jnz .a20_ok

    or al, 2
    and al, 0xFE
    out 0x92, al

.a20_ok:
    mov al, 'B'
    call printch
;load kernel from disk to 0x100000
    mov si, dap_kernel
    mov ah, 0x42
    mov dl, [boot_drive]
    
    int 0x13
    jc error
    mov al, 'C'
    call printch
    mov si, dap_pagetables
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jc error
    mov al, 'D'
    call printch

    ;load gdt and switch proc mode

    lgdt [gdtr]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:pm_entry
    
    ;the proc mode

[bits 32]
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    
    mov fs, ax 
    mov gs, ax
    mov ss, ax
    mov byte [0xB8000], 'P'

    mov esp, 0x90000

    cld
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, 0x2000
    rep movsb

    
    mov esi, 0x80000
    mov edi, 0x200000
    mov ecx, 0x3000
    rep movsb

    or eax, (1 << 5) | (1 << 7)
    mov cr4, eax
    mov eax, 0x200000

    mov cr3, eax
    mov ecx, 0xC0000080
    rdmsr

    or eax, (1 << 8)
    wrmsr
    mov eax, cr0

    or eax, (1 << 31)
    mov cr0, eax
    mov byte [0xB8002], 'Q'

    ;jump to 64 bit code hopefully
    jmp 0x18:lm_entry



    ;LONG MODE 64
[bits 64]
lm_entry:
    mov rax, 0xB8004
    mov byte [rax], 'L'
    mov rsp, 0x90000
    jmp 0x100000

; error handeling

error:
    mov si, msg_error
    call print16
    cli
    hlt
    jmp error

print16:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print16
.done:
    ret

printch:
    mov ah, 0x0E
    int 0x10
    ret
;da- i jsut remeberd why the fuck am i writing comments if i dont need them? its not like others will read them...

boot_drive db 0
msg_error db "Loader: error loading kernel or the pages :d", 13, 10, 0

dap_kernel:
    db 0x10
    db 0x00
    dw 16
    dw 0x0000
    dw 0x1000
    dd 2
    dd 0


dap_pagetables:
    db 0x10
    db 0x00
    dw 24
    dw 0x0000
    dw 0x8000
    dd 66
    dd 0


gdt:
    dq 0

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xAF
    db 0x00

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

gdt_end:

gdtr:
    dw gdt_end - gdt - 1
    dd gdt

    times 510 - ($ - $$) db 0
    dw 0xAA55