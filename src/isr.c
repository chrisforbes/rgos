#include "rgos.h"

static handler_f * handlers[48];

void isr_handler( struct regs regs )
{
	handler_f * f = handlers[regs.int_no];
	if (f)
		f( &regs );
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
}

void isr_register( int int_no, handler_f * f )
{
	handlers[int_no] = f;
}
