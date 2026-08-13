[bits 32]

%define MAP_ENTRIES 64
%define PRESENT_WRITE 0x03
%define PS_BIT 0x80

align 4096
PML4:
    dq 0x201000 | PRESENT_WRITE
    times 511 dq 0


align 4096
PDP:
    dq 0x202000 | PRESENT_WRITE
    times 511 dq 0

align 4096
PD:
    %assign i 0
    %rep MAP_ENTRIES
        dq (i * 0x200000) | PRESENT_WRITE | PS_BIT
    %assign i i+1
    %endrep
    %assign remaining 512 - MAP_ENTRIES
    %if remaining > 0
        times remaining dq 0
    %endif
    