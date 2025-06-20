#include "x86.h"
#include "device.h"
#include "common.h"

extern TSS tss;
extern ProcessTable pcb[MAX_PCB_NUM];
extern int current;

extern int displayRow;
extern int displayCol;

void GProtectFaultHandle(struct StackFrame *sf);
void timerHandle(struct StackFrame *sf);
void syscallHandle(struct StackFrame *sf);

void syscallWrite(struct StackFrame *sf);
void syscallPrint(struct StackFrame *sf);
void syscallFork(struct StackFrame *sf);
void syscallSleep(struct StackFrame *sf);
void syscallExit(struct StackFrame *sf);

void irqHandle(struct StackFrame *sf)
{ // pointer sf = esp
	/* Reassign segment register */
	asm volatile("movw %%ax, %%ds" ::"a"(KSEL(SEG_KDATA)));
	/*XXX Save esp to stackTop */
	uint32_t tmpStackTop = pcb[current].stackTop;
	pcb[current].prevStackTop = pcb[current].stackTop;
	pcb[current].stackTop = (uint32_t)sf;

	switch (sf->irq)
	{
	case -1:
		break;
	case 0xd:
		GProtectFaultHandle(sf);
		break;
	case 0x20:
		timerHandle(sf);
		break;
	case 0x80:
		syscallHandle(sf);
		break;
	default:
		assert(0);
	}
	/*XXX Recover stackTop */
	pcb[current].stackTop = tmpStackTop;
}

void GProtectFaultHandle(struct StackFrame *sf)
{
	assert(0);
	return;
}

void timerHandle(struct StackFrame *sf)
{
	// TODO
	putStr("Now current is ");
	putNum(current);
	putStr(", with state ");
	putNum(pcb[current].state);
	putStr(" , exec at eip ");
	putNum(pcb[current].regs.eip);
	putChar('\n');
		
	for (int i = 0; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state == STATE_BLOCKED)
		{
			pcb[i].sleepTime--;
			if (pcb[i].sleepTime == 0)
			{
				pcb[i].state = STATE_RUNNABLE;
				pcb[i].timeCount = 0;
				putStr("change blocked to runnable\n");
			}
		}
	}
	int next = -1;
	for (int i = (current + 1) % MAX_PCB_NUM; i != current; i = (i + 1) % MAX_PCB_NUM)
	{
		if (pcb[i].state == STATE_RUNNABLE)
		{
			next = i;
			putStr("next runnable: ");
			putNum(next);
			putChar('\n');
			break;
		}
	}
	if(pcb[current].state == STATE_RUNNING)
	{
		pcb[current].timeCount++;
		if(pcb[current].timeCount >= MAX_TIME_COUNT)
		{
			pcb[current].state = STATE_RUNNABLE;
			pcb[current].timeCount = 0;
		}
	}
	if ((pcb[current].state != STATE_RUNNING) && next != -1)
	{
		pcb[next].state = STATE_RUNNING;
		// switch process
		putStr("change pcb from ");
		putNum(current);
		putStr(" to ");
		putNum(next);
		putChar('\n');
		current = next;

		uint32_t tmpStackTop = pcb[current].stackTop;
		pcb[current].stackTop = pcb[current].prevStackTop;
		tss.esp0 = (uint32_t)&(pcb[current].stackTop);
		asm volatile("movl %0,%%esp" ::"m"(tmpStackTop));
		asm volatile("popl %gs");
		asm volatile("popl %fs");
		asm volatile("popl %es");
		asm volatile("popl %ds");
		asm volatile("popal");
		asm volatile("addl $8,%esp");
		asm volatile("iret");
	}
}

void syscallHandle(struct StackFrame *sf)
{
	switch (sf->eax)
	{ // syscall number
	case 0:
		syscallWrite(sf);
		break; // for SYS_WRITE
	/*TODO Add Fork,Sleep... */
	case 1:
		syscallFork(sf);
		break;
	case 3:
		syscallSleep(sf);
		break;
	case 4:
		syscallExit(sf);
		break;
	default:
		break;
	}
}

void syscallWrite(struct StackFrame *sf)
{
	switch (sf->ecx)
	{ // file descriptor
	case 0:
		syscallPrint(sf);
		break; // for STD_OUT
	default:
		break;
	}
}

void syscallPrint(struct StackFrame *sf)
{
	int sel = sf->ds; // segment selector for user data, need further modification
	char *str = (char *)sf->edx;
	int size = sf->ebx;
	int i = 0;
	int pos = 0;
	char character = 0;
	uint16_t data = 0;
	asm volatile("movw %0, %%es" ::"m"(sel));
	for (i = 0; i < size; i++)
	{
		asm volatile("movb %%es:(%1), %0" : "=r"(character) : "r"(str + i));
		if (character == '\n')
		{
			displayRow++;
			displayCol = 0;
			if (displayRow == 25)
			{
				displayRow = 24;
				displayCol = 0;
				scrollScreen();
			}
		}
		else
		{
			data = character | (0x0c << 8);
			pos = (80 * displayRow + displayCol) * 2;
			asm volatile("movw %0, (%1)" ::"r"(data), "r"(pos + 0xb8000));
			displayCol++;
			if (displayCol == 80)
			{
				displayRow++;
				displayCol = 0;
				if (displayRow == 25)
				{
					displayRow = 24;
					displayCol = 0;
					scrollScreen();
				}
			}
		}
		// asm volatile("int $0x20"); //XXX Testing irqTimer during syscall
		// asm volatile("int $0x20":::"memory"); //XXX Testing irqTimer during syscall
	}

	updateCursor(displayRow, displayCol);
	// take care of return value
	return;
}

// TODO syscallFork ...
void syscallFork(struct StackFrame *sf)
{
	// TODO
	int idx = -1;
	int i;
	for (i = 1; i < MAX_PCB_NUM; ++i)
	{
		if (pcb[i].state == STATE_DEAD)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
	{
		pcb[current].regs.eax = -1;
		return;
	}
	
	char *src = (char *)((current + 1) * 0x100000);
	char *dst = (char *)((idx + 1) * 0x100000);
	for (i = 0; i < 0x100000; ++i)
	{
		dst[i] = src[i];
	}
	
	for (i = 0; i < MAX_STACK_SIZE; ++i)
	{
		pcb[idx].stack[i] = pcb[current].stack[i];
	}

	pcb[idx].regs.ss = USEL(2 + idx * 2);
	pcb[idx].regs.esp = pcb[current].regs.esp;
	pcb[idx].regs.eflags = pcb[current].regs.eflags | 0x200;
	pcb[idx].regs.cs = USEL(1 + idx * 2);
	pcb[idx].regs.eip = pcb[current].regs.eip;
	pcb[idx].regs.ds = USEL(2 + idx * 2);
	pcb[idx].regs.es = USEL(2 + idx * 2);
	pcb[idx].regs.fs = USEL(2 + idx * 2);
	pcb[idx].regs.gs = USEL(2 + idx * 2);
	pcb[idx].regs.eax = 0; // 子进程的返回值为0
	//edi, esi, ebp, xxx, ebx, edx, ecx, eax;
	pcb[idx].regs.ecx = pcb[current].regs.ecx;
	pcb[idx].regs.edx = pcb[current].regs.edx;
	pcb[idx].regs.ebx = pcb[current].regs.ebx;
	pcb[idx].regs.xxx = pcb[current].regs.xxx;
	pcb[idx].regs.ebp = pcb[current].regs.ebp;
	pcb[idx].regs.esi = pcb[current].regs.esi;
	pcb[idx].regs.edi = pcb[current].regs.edi;
	pcb[idx].stackTop = (uint32_t)&(pcb[idx].regs);
	pcb[idx].prevStackTop = (uint32_t)&(pcb[idx].stackTop);
	pcb[idx].state = STATE_RUNNABLE;
	pcb[idx].timeCount = 0;
	pcb[idx].sleepTime = 0;
	pcb[idx].pid = idx;
	for (i = 0; i < 32; ++i)
	{
		pcb[idx].name[i] = pcb[current].name[i];
	}
	//sf->eax = idx;
	pcb[current].regs.eax = idx;
}

void syscallSleep(struct StackFrame *sf)
{
	// TODO
	putStr("In syscallSleep\n");
	if (sf->ecx <= 0)
	{
		return;
	}
	pcb[current].state = STATE_BLOCKED;
	pcb[current].sleepTime = sf->ecx;

	asm volatile("int $0x20");
}

void syscallExit(struct StackFrame *sf)
{
	// TODO
	putStr("In syscallExit\n");
	pcb[current].state = STATE_DEAD;

	asm volatile("int $0x20");
}
