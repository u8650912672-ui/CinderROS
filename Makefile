NASM := nasm
OBJCOPY := objcopy
IMG := cinder.img

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
KERNEL_BIN := $(BIOSDIR)/kernel.bin

.PHONY: all image img bins toolchain flash clean iso

all:
	@read -p "Heyyyy is this may be your first time, do you know what you are doing?  [y/N]: " ans; \
	case "$$ans" in y|Y|yes|Yes|YES) \
	    read -p "Do you want the iso or the .img? (iso/img) [iso]: " t; \
	    case "$$t" in img|IMG) $(MAKE) img;; *) $(MAKE) iso;; esac;; \
	*) echo "Making both (.img + .iso)"; $(MAKE) iso;; \
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
# Bootloader
$(BIOSDIR)/stage1.bin: boot/bios/stage1/start.asm
	@mkdir -p $(BIOSDIR)
	$(NASM) -f bin $< -o $@
$(BIOSDIR)/loader.bin: boot/bios/stage2/loader.asm
	@mkdir -p $(BIOSDIR)
	$(NASM) -f bin $< -o $@
$(BIOSDIR)/page_tables.bin: boot/bios/stage2/page_tables.asm
	@mkdir -p $(BIOSDIR)
	$(NASM) -f bin $< -o $@
# kernel
$(BIOSDIR)/kernel/%.o: kernel/%.c | check
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
$(BIOSDIR)/kernel/%.o: kernel/%.asm | check
	@mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@
$(KERNEL_BIN): $(KOBJS) kernel/linker.ld
	$(REQUIRE_CC)
	$(CC) -nostdlib -static $(KOBJS) -o $(KERNEL_ELF) \
	    -Wl,-m,elf_x86_64 -Wl,-T,kernel/linker.ld -lgcc
	$(OBJCOPY) -O binary $(KERNEL_ELF) $@
	@sz=$$(stat -c%s $@); \
	if [ $$sz -gt 32768 ]; then \
	    echo "ERROR: kernel.bin is somehow to big  ($$sz > 32768)."; \
	    echo "  raise 'dw 64' in loader.asm (dap_kernel) first."; \
	    exit 1; \
	fi
# bins no asking
bins: $(BIOSDIR)/stage1.bin $(BIOSDIR)/loader.bin $(BIOSDIR)/page_tables.bin $(KERNEL_BIN)
# interactive image
image: bins
	@rm -f $(IMG)
	@dd if=/dev/zero of=$(IMG) bs=512 count=128 status=none
	@for spec in \
	    "stage1     $(BIOSDIR)/stage1.bin       0 " \
	    "loader     $(BIOSDIR)/loader.bin       1 " \
	    "kernel     $(KERNEL_BIN)               2 " \
	    "pagetables $(BIOSDIR)/page_tables.bin 66"; do \
	    set -- $$spec; name=$$1; file=$$2; sec=$$3; \
	    read -p "Add $$name ($$file) at sector $$sec? [Y/n]: " ans; \
	    case "$$ans" in ""|y|Y|yes|Yes|YES) \
	        echo "  + $$file -> LBA $$sec"; \
	        dd if="$$file" of=$(IMG) bs=512 seek=$$sec conv=notrunc status=none;; \
	    *) echo "  - skipped $$name";; \
	    esac; \
	done
	@echo; echo "Image is ready: $(IMG)"

# non-interactive image (used by iso / img-only builds)
img: bins
	@rm -f $(IMG)
	@dd if=/dev/zero of=$(IMG) bs=512 count=128 status=none
	@for spec in \
	    "stage1     $(BIOSDIR)/stage1.bin       0 " \
	    "loader     $(BIOSDIR)/loader.bin       1 " \
	    "kernel     $(KERNEL_BIN)               2 " \
	    "pagetables $(BIOSDIR)/page_tables.bin 66"; do \
	    set -- $$spec; file=$$2; sec=$$3; \
	    echo "  + $$file -> LBA $$sec"; \
	    dd if="$$file" of=$(IMG) bs=512 seek=$$sec conv=notrunc status=none; \
	done
	@echo; echo "Image is done and ready: $(IMG)"

# grub iso
iso: bins img
	@rm -rf iso
	@mkdir -p iso/boot/grub
	@cp $(IMG) iso/cinder.img
	@cp boot/grub.cfg iso/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO) iso/
	@echo "ISO ready: $(ISO)"
# flash
flash: image
	@echo "MAKE SURE YOU KNOW THE CORRECT PATH OFTEN /dev/sda OR SIMULAR"
	@if [ -z "$(DEV)" ]; then echo "usage: make flash DEV=/dev/sdX"; exit 1; fi
	@[ -b "$(DEV)" ] || { echo "not a block device: $(DEV)"; exit 1; }
	@read -p "Overwrite $(DEV)? type yes: " a; \
	    [ "$$a" = "yes" ] || { echo aborted; exit 1; }
	dd if=$(IMG) of=$(DEV) bs=512 conv=notrunc status=progress

clean:
	rm -rf build $(IMG) $(ISO) iso