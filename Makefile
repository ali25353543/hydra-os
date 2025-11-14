# ==========================================
#   Simple OS Build Makefile
# ==========================================

# Source objects
OBJECTS   = loader.o kmain.o

# Compiler / Assembler / Linker
CC        = gcc
AS        = nasm
LD        = ld

# Flags
CFLAGS    = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
            -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS   = -f elf
LDFLAGS   = -T link.ld -melf_i386

# ==========================================
#   Targets
# ==========================================

all: kernel.elf

# Link all objects into the final kernel ELF
kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Build ISO image
os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R \
		-b boot/grub/stage2_eltorito \
		-no-emul-boot \
		-boot-load-size 4 \
		-A os \
		-input-charset utf8 \
		-quiet \
		-boot-info-table \
		-o os.iso \
		iso

# Run in Bochs
run: os.iso
	bochs -f bochsrc.txt -q

# Compile C source files to .o
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Assemble .s files to .o
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Clean build artifacts
clean:
	rm -rf *.o kernel.elf os.iso
