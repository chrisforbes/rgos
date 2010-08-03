#include "rgos.h"

void isr_handler( struct regs regs )
{
	vga_puts( "Unhandled interrupt: " );
	vga_put_hex( regs.int_no );
	vga_puts( ", code=" );
	vga_put_hex( regs.err_code );
	vga_puts( "\n" );
}
