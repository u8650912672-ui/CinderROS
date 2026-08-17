NASM := nasm
ISO := cinder.iso
GRUB_MKRESCUE := grub-mkrescue

BIOSDIR := build/bios
TOOLS := $(wildcard tools/x86_64-elf-tools-*)
XC_CC := $(firstword $(wildcard tools/bin/x86_64-elf-gcc))

CC := $(if $(XC_CC),$(XC_CC),$(shell command -v x86_64-elf-gcc || echo MISSING))
CFLAGS := -m64 -ffreestanding -mno-red-zone -mgeneral-regs-only \
          -fno-builtin -fno-stack-protector -fno-pic -fno-pie \
          -Wall -Wextra -O2 -I kernel/include
C_SRCS := $(shell find kernel -name '*.c')

ASM_SRCS := $(shell find kernel -name '*.asm')
KOBJS := $(patsubst kernel/%.c,$(BIOSDIR)/kernel/%.o,$(C_SRCS)) \
         $(patsubst kernel/%.asm,$(BIOSDIR)/kernel/%.o,$(ASM_SRCS))
KERNEL_ELF := build/kernel.elf

.PHONY: all check toolchain clean iso
all:
	@read -p "Heyyyy is this may be your first time, do you know what you are doing?  [y/N]: " ans; \
	case "$$ans" in y|Y|yes|Yes|YES) \
	     $(MAKE) iso;; \
	*) echo "no worries, building the iso for you"; $(MAKE) iso;; \
	esac

REQUIRE_CC = @if [ "$(CC)" = "MISSING" ] || [ ! -x "$(CC)" ]; then \
        echo "ERROR: x86_64-elf-gcc wasent found on this system (you may need to download it)"; \
        echo "  run:  make toolchain"; \
        exit 1; \
    fi
.PHONY: check
check:
	$(REQUIRE_CC)
TOOLCHAIN_URL := https://github.com/lordmilko/i686-elf-tools/releases/download/15.2.0/x86_64-elf-tools-linux.zip

toolchain:
	@mkdir -p tools
	@echo "downloading tools..."
	@cd tools && wget -O x86_64-elf-tools.zip $(TOOLCHAIN_URL)
	@cd tools && unzip -o -q x86_64-elf-tools.zip && rm x86_64-elf-tools.zip
	@echo "Tools ready you can now rerun make :3"
$(BIOSDIR)/kernel/%.o: kernel/%.c | check
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
$(BIOSDIR)/kernel/%.o: kernel/%.asm | check
	@mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@
$(KERNEL_ELF): $(KOBJS) kernel/linker.ld
	$(REQUIRE_CC)
	$(CC) -nostdlib -static -no-pie $(KOBJS) -o $@ \
	    -Wl,-m,elf_x86_64 -Wl,-T,kernel/linker.ld -lgcc
$(KERNEL_ELF_U): $(UEFI_KOBJS) kernel/linker.ld
	$(REQUIRE_CC)
	$(CC) -nostdlib -static -no-pie $(UEFI_KOBJS) -o $@ \
	    -Wl,-m,elf_x86_64 -Wl,-T,kernel/linker.ld -lgcc

iso: $(KERNEL_ELF)
	@rm -rf iso
	@mkdir -p iso/boot/grub
	@cp $(KERNEL_ELF) iso/boot/kernel.elf
	@cp boot/grub.cfg iso/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) iso/
	@echo "ISO ready: $(ISO)"
clean:
	rm -rf build $(ISO) iso