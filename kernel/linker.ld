ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .multiboot : ALIGN(8) {
        *(.multiboot)
    }
    .boottext : ALIGN(4096) {
        *(.boottext)
    }
    .text : ALIGN(4096) {
        *(.text .text.*)
    }
    .rodata : ALIGN(4096) { *(.rodata .rodata.*) }
    .data : ALIGN(4096) { *(.data .data.*) }
    .bss : ALIGN(4096) {
        __bss_start = .;
        *(COMMON)
        *(.bss .bss.*)
        __bss_end = .;
    }
    /DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) }
}