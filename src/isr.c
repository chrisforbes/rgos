#include "rgos.h"

static handler_f * handlers[48];
static char const * vector_names[] = { 
	"Divide by Zero",
	"Debug Exception",
	"Nonmaskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bounds Check",
	"Invalid Opcode",
	"Coprocessor not Available",
	"Double Fault",
	"Reserved",
	"Invalid TSS",
	"Segment Not Present",
	"Stack Exception",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"Coprocessor Error",		/* 16 */
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",					/* 32 */
	"IRQ0", "IRQ1",	"IRQ2",	"IRQ3",
	"IRQ4",	"IRQ5",	"IRQ6",	"IRQ7"
	"IRQ8", "IRQ9",	"IRQ10", "IRQ11",
	"IRQ12", "IRQ13", "IRQ14", "IRQ15" };
	

void isr_handler( struct regs regs )
{
	handler_f * f = handlers[regs.int_no];
	if (f)
		f( &regs );
	else
		PANIC( vector_names[ regs.int_no ] );
}

void irq_handler( struct regs regs )
{
	/* send EOI to the PIC */
	if (regs.int_no >= 40)
		outb( 0xa0, 0x20 );		// reset slave PIC
	outb( 0x20, 0x20 );			// reset master PIC
	
	handler_f * f = handlers[regs.int_no];
	if (f) 
		f( &regs );
	else
		PANIC( vector_names[ regs.int_no ] );
}

void isr_register( int int_no, handler_f * f )
{
	handlers[int_no] = f;
}

void enable_interrupts(void) { asm volatile( "sti" ); }
void disable_interrupts(void) { asm volatile( "cli" ); }
void halt(void) { asm volatile( "hlt" ); }
