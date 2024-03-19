#include "boot.h"

#define SECTSIZE 512

void bootMain(void) {
	//TODO
	/**
	 * This code represents the bootloader's entry point.
	 * It reads a sector from a specific memory address (0x8c00) into the destination (dst) buffer.
	 * Then, it sets the entry point of the bootloader to the address 0x8c00 and jumps to it.
	 */
	void *dst = (void *)0x8c00;
	readSect(dst, 1);
	void (*entry)(void) = (void *)0x8c00;
	entry();
}

/**
 * Waits for the disk to be ready.
 * This function continuously checks the status register of the disk controller
 * until the disk is ready for the next operation.
 */
void waitDisk(void) {
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

 
void readSect(void *dst, int offset) { // reading a sector of disk
	int i;
	waitDisk();
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}
