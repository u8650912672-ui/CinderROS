ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .text : ALIGN(4096) {
        *(.text.entry)  /* this must be first byte */
        *(.text .text.*)
    }
    .rodata : ALIGN(4096) { *(.rodata .rodata.*) }
    .data : ALIGN(4096) { *(.data .data.*) }
    .bss (NOLOAD) : ALIGN(4096) {
        *(COMMON)
        *(.bss .bss.*)
    }
    /DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) }
}
