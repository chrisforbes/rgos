#include "rgos.h"

static int ticks = 0;
static void timer_callback( struct regs * r __unused )
{
//	vga_puts( "Tick..." );
//	vga_put_hex( ticks++ );
//	vga_put( '\n' );

	++ticks;
}

void timer_init( u32 freq )
{
	isr_register( IRQ(0), timer_callback );
	u32 divisor = 1193180 / freq;
	
	outb( 0x43, 0x36 );
	outb( 0x40, divisor & 0xff );
	outb( 0x40, (divisor >> 8) & 0xff );
}
