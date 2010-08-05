#include "rgos.h"

void panic( char const * file, int line, char const * failure )
{
	vga_puts( "KERNEL PANIC at " );
	vga_puts( file );
	vga_puts( ":" );
	vga_put_dec( line );
	vga_puts( "\n\t  " );
	vga_puts( failure );
	
	for(;;)
	{
		disable_interrupts();
		halt();
	}
}
