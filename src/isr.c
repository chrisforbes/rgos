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
	vga_puts( "Registered interrupt handler: n=" );
	vga_put_hex( int_no );
	vga_puts( " f=" );
	vga_put_hex( (u32) f );
	vga_put( '\n' );
	
	handlers[int_no] = f;
}

void enable_interrupts(void) { asm volatile( "sti" ); }
void disable_interrupts(void) { asm volatile( "cli" ); }
void halt(void) { asm volatile( "hlt" ); }
