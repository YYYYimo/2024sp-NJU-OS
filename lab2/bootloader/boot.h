#ifndef BOOT_H
#define BOOT_H


struct ELFHeader {
	unsigned int   magic;       /**< Magic number identifying the file as an ELF file. */
	unsigned char  elf[12];     /**< Identification bytes. */
	unsigned short type;        /**< Type of the ELF file. */
	unsigned short machine;     /**< Target architecture of the ELF file. */
	unsigned int   version;     /**< Version of the ELF file. */
	unsigned int   entry;       /**< Entry point of the ELF file. */
	unsigned int   phoff;       /**< Program header table offset. */
	unsigned int   shoff;       /**< Section header table offset. */
	unsigned int   flags;       /**< Processor-specific flags. */
	unsigned short ehsize;      /**< Size of the ELF header. */
	unsigned short phentsize;   /**< Size of each program header entry. */
	unsigned short phnum;       /**< Number of program header entries. */
	unsigned short shentsize;   /**< Size of each section header entry. */
	unsigned short shnum;       /**< Number of section header entries. */
	unsigned short shstrndx;    /**< Index of the section header table entry that contains the section names. */
};

/* ELF32 Program header */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};

typedef struct ELFHeader ELFHeader;//typedef ,discard "struct"
typedef struct ProgramHeader ProgramHeader;

void waitDisk(void);

void readSect(void *dst, int offset);

/* I/O functions */
static inline char inByte(short port) {
	char data;
	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline int inLong(short port) {
	int data;
	asm volatile("in %1, %0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outByte(short port, char data) {
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

#endif
