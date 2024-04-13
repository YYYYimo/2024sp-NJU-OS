# Compiler and linker settings
CC = gcc
LD = ld

# Compiler flags
CFLAGS = -m32 -march=i386 -static \
	 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
	 -Wall -O2 -I./include -g -Wno-unused-variable

# Assembler flags
ASFLAGS = -m32

# Linker flags
LDFLAGS = -m elf_i386

# Find all C and assembly source files
KCFILES = $(shell find ./ -name "*.c")
KSFILES = $(shell find ./ -name "*.S")

# Generate object file names from source files
KOBJS = $(KCFILES:.c=.o) $(KSFILES:.S=.o)

# Target to build kmain.bin
kmain.bin: $(KOBJS)
	$(LD) $(LDFLAGS) -e kEntry -Ttext 0x00100000 -o kMain.elf $(KOBJS)
	@#objcopy -S -j .text -j .rodata -j .eh_frame -j .data -j .bss -O binary kMain.elf kMain.bin
	@#objcopy -O binary kMain.elf kMain.bin
	@../utils/genKernel.pl kMain.elf
	@#../utils/genKernel.pl kMain.bin

# Clean up generated files
clean:
	rm -rf $(KOBJS) kMain.elf kMain.bin
