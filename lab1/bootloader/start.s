/*
# TODO: This is lab1.1
# Real Mode Hello World 
.code16

.global start
start:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x7d00, %ax
	movw %ax, %sp # setting stack pointer to 0x7d00
	# TODO:通过中断输出Hello World
	pushw $13 # pushing the size to print into stack
	pushw $message # pushing the address of message into stack
	callw displayStr

loop:
	jmp loop

message:
	.string "Hello, World!\n\0"

displayStr:
	pushw %bp
	movw 4(%esp), %ax
	movw %ax, %bp
	movw 6(%esp), %cx
	movw $0x1301, %ax
	movw $0x000c, %bx
	movw $0x0000, %dx
	int $0x10
	popw %bp
	ret


# TODO: This is lab1.2
# Protected Mode Hello World 
.code16

.global start
start:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	# TODO:关闭中断
	cli


	# 启动A20总线
	inb $0x92, %al 
	orb $0x02, %al
	outb %al, $0x92

	# 加载GDTR
	data32 addr32 lgdt gdtDesc # loading gdtr, data32, addr32

	# TODO：设置CR0的PE位（第0位）为1
	movl %cr0, %eax
	orl $0x1, %eax
	movl %eax, %cr0 


	# 长跳转切换至保护模式
	data32 ljmp $0x08, $start32 # reload code segment selector and ljmp to start32, data32

.code32
start32:
	movw $0x10, %ax # setting data segment selector
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %ss
	movw $0x18, %ax # setting graphics data segment selector
	movw %ax, %gs
	
	movl $0x8000, %eax # setting esp
	movl %eax, %esp
	# TODO:输出Hello World
	movw $0x0d48, 0xb8000
	movw $0x0d65, 0xb8002
	movw $0x0d6c, 0xb8004
	movw $0x0d6c, 0xb8006
	movw $0x0d6f, 0xb8008
	movw $0x0d2c, 0xb800a
	movw $0x0d20, 0xb800c
	movw $0x0d57, 0xb800e
	movw $0x0d6f, 0xb8010
	movw $0x0d72, 0xb8012
	movw $0x0d6c, 0xb8014
	movw $0x0d64, 0xb8016
	movw $0x0d21, 0xb8018
	movw $0x000a, 0xb801a
	movw $0x0000, 0xb801c

loop32:
	jmp loop32

message:
	.string "Hello, World!\n\0"



.p2align 2
gdt: # 8 bytes for each table entry, at least 1 entry
	# .word limit[15:0],base[15:0]
	# .byte base[23:16],(0x90|(type)),(0xc0|(limit[19:16])),base[31:24]
	# GDT第一个表项为空
	.word 0,0
	.byte 0,0,0,0

	# TODO：code segment entry
	.word 0xffff,0
	.byte 0,0x9a,0xcf,0

	# TODO：data segment entry
	.word 0xffff,0
	.byte 0,0x92,0xcf,0

	# TODO：graphics segment entry
	.word 0xffff,0x8000
	.byte 0x0b,0x92,0xcf,0

gdtDesc: 
	.word (gdtDesc - gdt -1) 
	.long gdt 
*/

#TODO: This is lab1.3
/* Protected Mode Loading Hello World APP */
.code16

.global start
start:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	# TODO:关闭中断
	cli

	# 启动A20总线
	inb $0x92, %al 
	orb $0x02, %al
	outb %al, $0x92

	# 加载GDTR
	data32 addr32 lgdt gdtDesc # loading gdtr, data32, addr32

	# TODO：设置CR0的PE位（第0位）为1
	movl %cr0, %eax
	orl $0x1, %eax
	movl %eax, %cr0 


	# 长跳转切换至保护模式
	data32 ljmp $0x08, $start32 # reload code segment selector and ljmp to start32, data32

.code32
start32:
	movw $0x10, %ax # setting data segment selector
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %ss
	movw $0x18, %ax # setting graphics data segment selector
	movw %ax, %gs
	
	movl $0x8000, %eax # setting esp
	movl %eax, %esp
	jmp bootMain # jump to bootMain in boot.c

.p2align 2
gdt: # 8 bytes for each table entry, at least 1 entry
	# .word limit[15:0],base[15:0]
	# .byte base[23:16],(0x90|(type)),(0xc0|(limit[19:16])),base[31:24]
	# GDT第一个表项为空
	.word 0,0
	.byte 0,0,0,0

	# TODO：code segment entry
	.word 0xffff,0
	.byte 0,0x9a,0xcf,0

	# TODO：data segment entry
	.word 0xffff,0
	.byte 0,0x92,0xcf,0

	# TODO：graphics segment entry
	.word 0xffff,0x8000
	.byte 0x0b,0x92,0xcf,0

gdtDesc: 
	.word (gdtDesc - gdt - 1) 
	.long gdt 
