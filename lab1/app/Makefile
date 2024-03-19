# Target: app.bin
# Description: This target compiles the assembly file app.s into a binary file app.bin.
app.bin: app.s
	gcc -c -m32 app.s -o app.o
	ld -m elf_i386 -e start -Ttext 0x8c00 app.o -o app.elf
	objcopy -S -j .text -O binary app.elf app.bin

clean:
	rm -rf *.o *.elf *.bin
