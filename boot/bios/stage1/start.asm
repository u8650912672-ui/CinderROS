;start of stage 1 and a new beginning :3

[org 0x7C00]  ;bios load
[bits 16]  ; Real mode 

; Nr1 start of segments and stack 
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00 ;stack grows down from 0x7C00

    ;nr 2 set text mode IF THIS DONT WORK MANUALLY CHANGE IT TO mov ax, 0x0003 
    call set_80x50
    ;nr3 obselete i will cahnge and erase it later 
    mov si, msg_boot
    call print
    ;nr 3 real load stage 2 from disk try lab first the fallback to CHS
    mov si, disk_address_packet
    mov ah, 0x42
    mod dl, [boot_drive]
    int 0x13
    jnc .load_ok

    ;if lba not supported it will do this (CHS FALLBACK)
    mov ax, 0x0201
    mov bx, 0x7E00
    mov cx, 0x0002
    mov dh, 0
    mov dl, [boot_drive]
    int 0x13
    jc error

.load_ok
    ;stage2 loaded seccessfully at 0x0000:0x7E0
    jmp 0x0000:ex7E00

    ;nr 4 error handeling
error:
    mov si, msg_error
    call print
    cli
    hlt
    jmp error ;in case of NMI or wakeup :p

    ;nr 5 helper and i/o without o
print:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

    ;nr 6set the vga 80x50
set_80x50
    mov ax, 0x0003
    int 0x10

    mov ax, 0x1112
    mov bl, 0x00
    int 0x10

    mov dx, 0x3D4
    mov al, 0x09
    out dx, al

    inc dx
    in al, dx
    and al, 0xE0

    or al, 0x07
    out dx, al

    ;set vertical display end to 400 scanlines
    dec dx
    mov al, 0x12
    out dx, al

    inc dx
    mov al, 0x8F
    out dx, al

    ;set a overflow register ( 8 bit of vertical)
    dec dx
    mov al, 0x07
    out dx, al

    inc dx
    in al,<
