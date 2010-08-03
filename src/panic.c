#include "rgos.h"

void panic( char const * file, char const * line, char const * failure )
{
	vga_puts( "KERNEL PANIC\n\tat " );
	vga_puts( file );
	vga_puts( ":" );
	vga_puts( line );
	vga_puts( "\n\t  " );
	vga_puts( failure );
	
	for(;;)
	{
		disable_interrupts();
		halt();
	}
}
